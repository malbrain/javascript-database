var debug = 0;

print("mongod is listening on tcp port 27017");
jsdb_tcpListen(27017, newConnection);

function newConnection (filein, fileout, connId) {
    print ("new connection accepted");
    var len, id, resp, op;
    var next, nonstop = 1;
	var catalog = {};
    var respid = 0;

    do {
        if (jsdb_readInt32(filein, &len, &id, &resp, &op))
            len -= 4 * 4;
        else
            break;

        if (debug) {
            print ("Len: ", len);
            print ("ReplyTo: ", id);
            print ("ResponseTo: ", resp);
            print ("Opcode: ", op);
        }

        respid += 1;

        if(op == 2010)
            newCommand(filein, fileout, len, respid, id, connId, catalog);

        if(op == 2004)
            newQuery(filein, fileout, len, respid, id, connId, catalog);

        if(op == 2002)
            newInsert(filein, fileout, len, respid, id, connId, catalog);

    } while (nonstop);
}

function newCommand (filein, fileout, len, respid, id, connId, catalog) {
    var count, docId, docStore, dbname, commandname;
    var db, collection, result, database;
    var flags, size, document, names;
    var array;

    if (jsdb_readString(filein, &dbname, &size)) {
        if(debug) print ("Command dbname: ", dbname);
    } else
        print ("Error on dbname");

    len -= size + 1;

    if (jsdb_readString(filein, &commandname, &size)) {
        if(debug) print ("Command commandname: ", commandname);
    } else
        print ("Error on commandname");

	if (debug) print("newCommand command: ", commandname);
    len -= size + 1;

    if(debug)
        print("read bson command bytes left = ", len);

    if (jsdb_readBSON(filein, &document, len, &size, &count)) {
        if(debug) print ("command document len = ", size);
    } else
        print ("Error on command document");

    if (debug)
        print (document);

    len -= size;

    if (commandname == "insert") {
    	if (database = catalog[dbname]) {
        	if(debug) print ("Database: ", database);
    	} else {
        	if(debug) print ("newDatabase: ", dbname);
        	jsdb_initDatabase(&db, dbname, 4096, false);
        	database = { db : db };
        	catalog[dbname] = database;
    	}

        if (collection = database[document[0].insert]) {
            if(debug) print ("Collection: ", collection);
            docStore = collection.docStore;
        } else {
            if(debug) print ("newCollection: ", document[0].insert);
            jsdb_createDocStore (&docStore, db, document[0].insert, 1024 * 1024, true);

            collection = { docStore : docStore };
            database[document[0].insert] = collection;
        }

        return insert(fileout, respid, id, collection.docStore, 2011, document[0].documents);
    }

    if (commandname == "find") {
    	if (database = catalog[dbname]) {
        	if(debug) print ("Database: ", database);
    	} else {
        	if(debug) print ("newDatabase: ", dbname);
        	jsdb_initDatabase(&db, dbname, 4096, false);
        	database = { db : db };
        	catalog[dbname] = database;
    	}

        if (collection = database[document[0].find]) {
            if(debug) print ("Collection: ", collection);
            docStore = collection.docStore;
        } else {
            if(debug) print ("newCollection: ", document[0].find);
            jsdb_createDocStore (&docStore, db, document[0].find, 1024 * 1024, true);

            collection = { docStore : docStore };
            database[document[0].find] = collection;
        }

        return jsdb_findDocs(fileout, docStore, document[0].filter, respid, id, 2011);
    }

    if (commandname == "whatsmyuri")
        return whatsmyuri(fileout, respid, id, 2011);
    
    if (commandname == "isMaster")
        return isMaster(fileout, respid, id, 2011);

    if (commandname == "getLog")
        return getLog(fileout, respid, id, 2011);

    if (commandname == "buildInfo")
        return buildInfo(fileout, respid, id, 2011);

    if (commandname == "replSetGetStatus")
        return replSetGetStatus(fileout, respid, id, 2011);
}

function newInsert (filein, fileout, len, respid, id, connId, catalog) {
    var count, docId, docStore, fullname;
    var flags, size, documents, names;
    var db, collection, result;
    var array;

    if (jsdb_readInt32(filein, &flags)) {
        if(debug) print ("Flags: ", flags);
    } else
        return;

    len -= 4;

    if (jsdb_readString(filein, &fullname, &size)) {
        if(debug) print ("Insert fullname: ", fullname);
    } else
        print ("Error on fullname");

    len -= size + 1;

    if(debug)
        print("read bson insert bytes left = ", len);

    if (jsdb_readBSON(filein, &documents, len, &size, &count)) {
        if(debug) print ("Insert document len = ", size);
    } else
        print ("Error on Insert document");

    if (debug)
        print (documents);

    if (jsdb_getTokens(fullname, ".", &names))
        if(debug) print ("fullname: ", names);

    if (db = catalog[names[0]]) {
        if(debug) print ("Database: ", db);
    } else {
        if(debug) print ("newDatabase: ", names[0]);
        jsdb_initDatabase(&db, names[0], 4096, false);
        catalog[names[0]] = db;
    }

    if (collection = db[names[1]]) {
        if(debug) print ("Collection: ", collection);
        docStore = collection.docStore;
    } else {
        if(debug) print ("newCollection: ", names[1]);
        jsdb_createDocStore (&docStore, db, names[1], 1024 * 1024, true);

        collection = {
                     name : names[1],
                     docStore : docStore
                     };

        db[names[1]] = collection;
    }

    if (jsdb_insertDocs(docStore, documents, &docId, &count))
       if(debug) print ("inserted = ", count);
}

function newQuery (filein, fileout, len, respid, id, connId, catalog) {
    var count, docId, docStore, fullname;
    var flags, nskip, nreturn, names;
    var size, query, selector;
    var result, array, docs;
    var db, collection;

    if (jsdb_readInt32(filein, &flags)) {
        if(debug) print ("Flags: ", flags);
    } else
        return;

    len -= 4;
	if (debug) print ("Len: ", len);

    if (jsdb_readString(filein, &fullname, &size)) {
        if(debug) print ("Query fullname: ", fullname, " size: ", size);
    } else
        print ("Error on fullname");

    len -= size + 1;
	if (debug) print ("Len: ", len);

    if (jsdb_readInt32(filein, &nskip)) {
        if(debug) print ("Query num skip: ", nskip);
    } else
        print ("Error on nskip");

    if (jsdb_readInt32(filein, &nreturn)) {
        if (debug) print ("Query num return: ", nreturn);
    } else
        print ("Error on nreturn");

    len -= 2 * 4;

    if(debug)
        print("read bson query bytes left = ", len);

    if (jsdb_readBSON(filein, &docs, len, &size, &count)) {
        if(debug) print ("Query document len = ", size);
    } else
        print ("Error on query document");

    query = docs[0];

    if (count > 1)
        selector = docs[1];

    if (debug)
        print (query);

    len -= size;

    if (debug)
        print ("finish query command, bytes remaining = ", len);

    if (jsdb_getTokens(fullname, ".", &names))
        if(debug) print ("fullname: ", names);

    if (names[0] == "admin")
     if( names[1] == "$cmd")
        return adminCommand(query, fileout, respid, id, connId, catalog);

    if (db = catalog[names[0]]) {
        if(debug) print ("Database: ", db);
    } else {
        if(debug) print ("newDatabase: ", names[0]);
        jsdb_initDatabase(&db, names[0], 4096, false);
        catalog[names[0]] = db;
    }

    //  if this is a $cmd operation

    if( names[1] == "$cmd")
        return dbCommand(query, db, fileout, respid, id, connId, catalog, query);

    //  otherwise it's a find operation

    if(debug) print("catalog =", catalog, "  collection=",db[names[1]]);

    if (collection = db[names[1]]) {
        if(debug) print ("Collection: ", collection.name);
        docStore = collection.docStore;
    } else {
        if(debug) print ("newCollection: ", names[1]);
        jsdb_createDocStore (&docStore, db, names[1], 1024 * 1024, true);

        collection = {
                      name : names[1],
                      docStore : docStore
                     };

        db[names[1]] = collection;
    }

    if(debug) print("db[", names[1], "] = ", collection);
    findDocs(fileout, docStore, query, respid, id, 1);
}

function findDocs(fileout, docStore, query, respid, id, opcode) {
    var iterator, document, docId, out = 0, incl;
    var array = [];

    jsdb_createIterator(docStore, &iterator);

    while (jsdb_nextDoc(iterator, &docId, &document))
        if (jsdb_findDocs(query, document, &incl))
            if (incl) {
                array[out] = document;
                out += 1;
            }

    if(debug) print("response ID:", respid, "  responding to:", id);
    jsdb_response(fileout, respid, id, 8, 0, opcode, 0, array);
    return;
}

function adminCommand(query, fileout, respid, id, connId, catalog) {
    var result, array;

    if (debug) print("adminCommand: ", query);

    if (query.isMaster)
        return isMaster(fileout, respid, id, 1);

    if (query.buildInfo)
        return buildInfo(fileout, respid, id, 1);

    if (query.replSetGetStatus)
        return replSetGetStatus(fileout, respid, id, 1);

    if (query.getLog)
        return getLog(fileout, respid, id, 1);

    if (query.whatsmyuri)
        return whatsmyuri(fileout, respid, id, 1);
}

function dbCommand(query, db, fileout, respid, id, connId, catalog, query) {
    var result, array, docStore;
    var collection, docId, count;

    if (query.isMaster)
        return isMaster(fileout, respid, id, 1);

    if (query.count)
        return dbCount(fileout, respid, id, 1, db, query);

    if (query.buildInfo)
        return buildInfo(fileout, respid, id, 1);

    if (query.getlasterror) {
        result = {
                  n : 1,
                  connectionId : connId,
                  wtime : 0,
                  ok : 1
                 };

        array = [ result ];
        if(debug) print(array);

        if(debug) print("response ID:", respid, "  responding to:", id);
        jsdb_response(fileout, respid, id, 8, 0, 1, 0, array);
        return;
    }

    if (query.insert) {
        if(debug) print("insert: ", query.insert);
    } else
        return;

    if (collection = db[query.insert]) {
        if(debug) print ("Collection: ", collection);
        docStore = collection.docStore;
    } else {
        if(debug) print ("newCollection: ", query.insert);
        jsdb_createDocStore (&docStore, db, query.insert, 1024 * 1024, true);

        collection = {
                     name : query.insert,
                     docStore : docStore
                     };

        db[query.insert] = collection;
    }

    return insert(fileout, respid, id, collection.docStore, 1, query.documents);
}

function insert(fileout, respid, id, docStore, opcode, documents) {
    var count, docId, result;

    if (jsdb_insertDocs(docStore, documents, &docId, &count))
       if(debug) print ("inserted = ", count);

    if (opcode == 1)
        result = {
              writeErrors : [],
              writeConcernErrors : [],
              nInserted : count,
              nUpserted : 0,
              nMatched : 0,
              nModified : 0,
              nRemoved : 0,
              upserted : 0
              };
    else
        result = {
              ok : 1,
              n : count
              };
        
    var array = [ result ];
    if(debug) print(array);

    if(debug) print("response ID:", respid, "  responding to:", id);
    jsdb_response(fileout, respid, id, 8, 0, opcode, 0, array);
    return;
}

function whatsmyuri(fileout, respid, id, opcode) {
    var result = {
                 you : "127.0.0.1:59000",
                 ok : 1
               };

    var array = [ result ];
    if(debug) print(array);

    if(debug) print("response ID:", respid, "  responding to:", id);
    jsdb_response(fileout, respid, id, 8, 0, opcode, 0, array);
}

function isMaster(fileout, respid, id, opcode) {
    var result = {
                 isMaster : 1,
                 maxBsonObjectSize : 16777216,
                 maxMessageSizeBytes : 48000000,
                 maxWriteBatchSize : 1000,
                 localTime : 12345678,
                 maxWireVersion : 4,
                 minWireVersion : 0,
                 readOnly : 0,
                 ok : 1
                };

    var array = [ result ];
    if(debug) print(array);

    if(debug) print("response ID:", respid, "  responding to:", id);
    jsdb_response(fileout, respid, id, 8, 0, opcode, 0, array);
}

function buildInfo(fileout, respid, id, opcode) {
    var result = {
                  version : "3.3.1-269-gda2441b",
                  targetMinOS : "Windows 7",
                  allocator : "jsdb arena",
                  javascriptEngine : "jsdb script",
                  versionArray : [ 3, 3, 2, -100 ],
                  bits : 64,
                  debug : 1,
                  ok : 1
                 };

    var array = [ result ];
    if(debug) print(array);

    if(debug) print("response ID:", respid, "  responding to:", id);
    jsdb_response(fileout, respid, id, 8, 0, opcode, 0, array);
    return;
}

function getLog(fileout, respid, id, opcode) {
    var result = {
              totalLinesWritten : 0,
              log : [],
              ok : 1
             };

    var array = [ result ];
    if(debug) print(array);

    if(debug) print("response ID:", respid, "  responding to:", id);
    jsdb_response(fileout, respid, id, 8, 0, opcode, 0, array);
    return;
}

function buildInfo(fileout, respid, id, opcode) {
    var result = {
                  version : "3.3.1-269-gda2441b",
                  targetMinOS : "Windows 7",
                  allocator : "jsdb arena",
                  javascriptEngine : "jsdb script",
                  versionArray : [ 3, 3, 2, -100 ],
                  bits : 64,
                  debug : 1,
                  ok : 1
                 };

    var array = [ result ];
    if(debug) print(array);

    if(debug) print("response ID:", respid, "  responding to:", id);
    jsdb_response(fileout, respid, id, 8, 0, opcode, 0, array);
    return;
}

function replSetGetStatus(fileout, respid, id, opcode) {
    var result = {
                  errmsg : "not running with --replSet",
                  code : 76,
                  ok : 0,
                 };

    var array = [ result ];
    if(debug) print(array);

    if(debug) print("response ID:", respid, "  responding to:", id);
    jsdb_response(fileout, respid, id, 8, 0, opcode, 0, array);
    return;
}

function dbCount(fileout, respid, id, opcode, db, query)
{
    var collection, docStore;

    if (collection = db[query.count]) {
        if(debug) print ("Collection: ", collection);
        docStore = collection.docStore;
    } else {
        if(debug) print ("newCollection: ", query.count);
        jsdb_createDocStore (&docStore, db, query.insert, 1024 * 1024, true);

        collection = {
                     name : query.insert,
                     docStore : docStore
                     };

        db[query.count] = collection;
    }

    var iterator, document, docId, count = 0, incl;

    jsdb_createIterator(docStore, &iterator);

    while (jsdb_nextDoc(iterator, &docId, &document))
        if (jsdb_findDocs(query.query, document, &incl))
            if (incl)
                count += 1;

    var result = {
                  n : count,
                  ok : 1
                 };

    var array = [ result ];
    if(debug) print(array);

    if(debug) print("response ID:", respid, "  responding to:", id);
    jsdb_response(fileout, respid, id, 8, 0, 1, 0, array);
}
