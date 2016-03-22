var debug = 1;

print("mongod is listening on tcp port 27017");
jsdb_tcpListen(27017, newConnection);

function newConnection (filein, fileout, connId) {
	print ("new connection accepted");
	var len, id, resp, op, collAuto;
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
			newCommand();

		if(op == 2004)
			newQuery();

		if(op == 2002)
			newInsert();

	} while (nonstop);

  //  the sub-functions

  function newCommand () {
	var count, docId, docStore, dbname, commandname;
	var db, docStore, result, database;
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
		docStore = getCollection (dbname, document[0].insert);
		return insert(2011, docStore, document[0].documents);
	}

	if (commandname == "find") {
		docStore = getCollection (dbname, document[0].find);
		return jsdb_findDocs(fileout, docStore, document[0].filter, respid, id, 2011);
	}

	if (commandname == "createIndexes") {
		docStore = getCollection (dbname, document[0].createIndexes);
		return createIndexes(2011, docStore, document[0].indexes);
	}
	
	if (commandname == "whatsmyuri")
		return whatsmyuri(2011);
	
	if (commandname == "isMaster")
		return isMaster(2011);

	if (commandname == "getLog")
		return getLog(2011);

	if (commandname == "buildInfo")
		return buildInfo(2011);

	if (commandname == "replSetGetStatus")
		return replSetGetStatus(2011);
  }

  function newInsert () {
	var count, docId, docStore, fullname;
	var flags, size, documents, names;
	var array, result;

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

	if (getTokens(fullname, ".", &names))
		if(debug) print ("fullname: ", names);

	docStore = getCollection(names[0], names[1]);

	if (jsdb_insertDocs(docStore, documents, &docId, &count))
	   if(debug) print ("inserted = ", count);
  }

  function newQuery () {
	var count, docId, docStore, fullname;
	var flags, nskip, nreturn, names;
	var size, query, selector;
	var result, array, docs;
	var collection;

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

	if (getTokens(fullname, ".", &names))
		if(debug) print ("fullname: ", names);

	if (names[0] == "admin")
	 if( names[1] == "$cmd")
		return adminCommand(query);

	//  if this is a $cmd operation

	if( names[1] == "$cmd")
		return dbCommand(names[0], query);

	//  otherwise it's a find operation

	if(debug) print("find: ", names[0], ".", names[1], " query: ", query);

	docStore = getCollection (names[0], names[1]);
	findDocs(docStore, query, 1);
  }

  function findDocs(docStore, query, opcode) {
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

  function adminCommand(query) {
	if (debug) print("adminCommand: ", query);

	if (query.isMaster)
		return isMaster(1);

	if (query.buildInfo)
		return buildInfo(1);

	if (query.replSetGetStatus)
		return replSetGetStatus(1);

	if (query.getLog)
		return getLog(1);

	if (query.whatsmyuri)
		return whatsmyuri(1);
  }

  function dbCommand(dbname, query) {
	var result, array, docStore;
	var docId, count;

	if (query.isMaster)
		return isMaster(1);

	if (query.count)
		return dbCount(1, dbname, catalog, query);

	if (query.buildInfo)
		return buildInfo(1);

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
		docStore = getCollection (dbname, query.insert);
		return insert(1, docStore, query.documents);
	}
  }

  function insert(opcode, docStore, documents) {
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

  function whatsmyuri(opcode) {
	var result = {
				 you : "127.0.0.1:59000",
				 ok : 1
			   };

	var array = [ result ];
	if(debug) print(array);

	if(debug) print("response ID:", respid, "  responding to:", id);
	jsdb_response(fileout, respid, id, 8, 0, opcode, 0, array);
  }

  function isMaster(opcode) {
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

  function buildInfo(opcode) {
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

  function getLog(opcode) {
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

  function replSetGetStatus(opcode) {
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

  function dbCount(opcode, dbname, catalog, query) {
	var docStore = getCollection (dbname, query.count);
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

  function getCollection (dbname, collname) {
	var db, database, collection, docStore, index;

   	if (database = catalog[dbname]) {
	   	if(debug) print ("Database: ", database);
   	} else {
	   	if(debug) print ("newDatabase: ", dbname);
	   	jsdb_initDatabase(&db, dbname, 4096, false);
	   	database = { db : db };
	   	catalog[dbname] = database;
   	}

	if (collection = database[collname]) {
		if(debug) print ("Collection: ", collection);
	} else {
		if(debug) print ("newCollection: ", collname);
		jsdb_createDocStore (&docStore, db, collname, 1024 * 1024, true, &collAuto);

		if (collAuto)
		  if( jsdb_createIndex(docStore, &index, { _id:1 }, "_id_", "art", 0, true, true, false, null)) {
		  } else
			print("create _id index error: ", index);

		collection = { docStore : docStore, idIdx : index };
		database[collname] = collection;
	}

	return collection.docStore;
  }

  function createIndexes(opcode, docStore, indexes) {
	var idx = 0, index, result, array, hndl;

	while (index = indexes[idx]) {
		if(debug) print ("create index #: ", idx, " -- ", index);
		if (jsdb_createIndex(docStore, &hndl, index.key, index.name, index.type, index.size, index.onDisk, index.unique, index.partialFilterExpression))
			idx += 1;
		else {
			print("createIndex error: ", index.name);
			idx += 1;
		}
	}

	var result = {
				 createdCollectionAutomatically : collAuto,
				 numIndexesBefore : 1,
				 numIndexesAfter : 2,
				 ok : 1
			   };

	var array = [ result ];
	if(debug) print(array);

	if(debug) print("response ID:", respid, "  responding to:", id);
	jsdb_response(fileout, respid, id, 8, 0, 2011, 0, array);
  }
}
