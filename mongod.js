var debug = 0;
var Catalog = {};

print("mongod is listening on tcp port 27017");
jsdb_tcpListen(27017, newConnection);

function newConnection (filein, fileout, connId) {
	print ("new connection accepted");
	var len, id, resp, op, collAuto;
	var next, nonstop = 1;
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
	var docId, docStore, dbname, commandname;
	var db, result, database;
	var flags, size, document, names;
	var array;

	if (jsdb_readString(filein, &dbname)) {
		if(debug) print ("Command dbname: ", dbname);
	} else
		print ("Error on dbname");

	len -= dbname.length + 1;

	if (jsdb_readString(filein, &commandname)) {
		if(debug) print ("Command commandname: ", commandname);
	} else
		print ("Error on commandname");

	if (debug) print("newCommand command: ", commandname);
	len -= commandname.length + 1;

	if(debug)
		print("read bson command bytes left = ", len);

	if (jsdb_readBSON(filein, &document, len, &size)) {
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
		return findDocs(docStore, document[0].filter, document[0].sort, 2011, dbname + "." + document[0].find);
	}

	if (commandname == "createIndexes") {
		if(debug) print("createCollection ", document[0].createIndexes, " for ", dbname);
		if(debug) print("repeat ", document[0].createIndexes, " for ", dbname);
		docStore = getCollection (dbname, document[0].createIndexes);
		if(debug) print("createIndexes on ", document[0].indexes);
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
	var docId, docStore, fullname;
	var flags, documents, names;
	var count, size, array, result;

	if (jsdb_readInt32(filein, &flags)) {
		if(debug) print ("Flags: ", flags);
	} else
		return;

	len -= 4;

	if (jsdb_readString(filein, &fullname)) {
		if(debug) print ("Insert fullname: ", fullname);
	} else
		print ("Error on fullname");

	len -= fullname.length + 1;

	if(debug)
		print("read bson insert bytes left = ", len);

	if (jsdb_readBSON(filein, &documents, len, &size)) {
		if(debug) print ("Insert document len = ", size);
	} else
		print ("Error on Insert document");

	if (debug)
		print (documents);

	names = fullname.split(".");
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

	if (jsdb_readInt32(filein, &flags)) {
		if(debug) print ("Flags: ", flags);
	} else
		return;

	len -= 4;
	if (debug) print ("Len: ", len);

	if (jsdb_readString(filein, &fullname)) {
		if(debug) print ("Query fullname: ", fullname, " size: ", fullname.length);
	} else
		print ("Error on fullname");

	len -= fullname.length + 1;
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

	if (jsdb_readBSON(filein, &docs, len, &size)) {
		if(debug) print ("Query document len = ", size);
	} else
		print ("Error on query document");

	query = docs[0];

	if (docs.length > 1)
		selector = docs[1];

	if (debug)
		print (query);

	len -= size;

	if (debug)
		print ("finish query command, bytes remaining = ", len);

	names = fullname.split(".");
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
	findDocs(docStore, query, {}, 1, fullname);
  }

  function findDocs(docStore, query, sort, opcode, fullname) {
	var iterator, document, docId, out = 0, incl, cursor;
	var array = [];

	if (debug) print("findDoc:", docStore, "\nquery:", query, "\nsort:", sort, "\nname:", fullname);

	if (!sort) {
		iterator = jsdb_createIterator(docStore.docStore);

		while (docId = jsdb_nextDoc(iterator, &document))
			if (jsdb_findDocs(query, document))
				array[out++] = document;
	} else {
		if (debug) print("createCursor: ", docStore[sort.index], "\nflds:", sort.fields);
		cursor = jsdb_createCursor(docStore[sort.index], true, sort.start, sort.limit);

		while (docId = jsdb_nextKey(cursor, docStore.docStore, &document))
			if (jsdb_findDocs(query, document))
				array[out++] = document;
	}

	var response = [{waitedMS : 10, cursor: { firstBatch : array, id: 0, ns: fullname}, ok : 1 }];

	if(debug) print("response ID:", respid, "  responding to:", id);
	jsdb_response(fileout, respid, id, 8, 0, opcode, 0, response);
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
		return dbCount(1, dbname, query);

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

	iterator = jsdb_createIterator(docStore.docStore);

	while (docId = jsdb_nextDoc(iterator, &document))
		if (jsdb_findDocs(query.query, document))
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
	var docStore, index;
	var name = dbname + "." + collname;

	if (debug) print("getCollection: ", name);

	if (docStore = Catalog[name]) {
		if(debug) print ("Collection: ", docStore);
	} else {
		docStore = jsdb_createDocStore (name, 1024 * 1024, true, &collAuto);

		if(debug) print (name, " newCollection: ", docStore);

		if (!docStore._id_)
		  if( index = jsdb_createIndex(docStore, { _id:1 }, "_id_", "art", 0, true, true, false, null)) {
			docStore._id_ = index;
			if(debug) print("create _id index: ", docStore);
		  } else
			print("create _id index error: ", name);

		Catalog[name] = docStore;
	}

	return docStore;
  }

  function createIndexes(opcode, docStore, indexes) {
	var before = Object.keys(docStore).length - 1;
	var idx = 0, index, result, array, hndl;

	while (index = indexes[idx]) {
		if(debug) print ("create index #: ", idx, " -- ", index);
		if (hndl = jsdb_createIndex(docStore, index.key, index.name, index.type, index.size, index.unique, index.partialFilterExpression)) {
			docStore[index.name] = hndl;
			idx += 1;
		} else {
			print("createIndex error: ", index.name);
			idx += 1;
		}
	}

	var after = Object.keys(docStore).length - 1;
	var result = {
				 createdCollectionAutomatically : collAuto,
				 numIndexesBefore : before,
				 numIndexesAfter : after,
				 ok : 1
			   };

	var array = [ result ];
	if(debug) print(array);

	if(debug) print("response ID:", respid, "  responding to:", id);
	jsdb_response(fileout, respid, id, 8, 0, 2011, 0, array);
  }
}
