var debug = true;

var Db = {};

Db.catalog = {};
Db.inMem = [];

Db._defaultSize = 1024 * 1024;
Db._autoIndexId = true;
Db._onDisk = true;

Db._getCollection = function(env, collName) {
	var name = env.dbname + "." + collName;
	var docStore, auto, index;

	env.auto = false;

	if (docStore = Db.catalog[name]) {
		if (debug) print(" foundCollection: ", name);
		return docStore;
	}

	if (debug) print(" newCollection: ", name);

	docStore = jsdb_createDocStore(name, Db._defaultSize, Db._onDisk, &auto); 
	env.auto = auto;

	if (!Db._onDisk)
		Db.inMem.push(name);

	if (!docStore._id_ && Db._autoIndexId)
		if (index = jsdb_createIndex(docStore, {_id:1}, "_id_", "art", 0, true, false))
			docStore._id_ = index;

	Db.catalog[name] = docStore;
};

Db.create = function(env, doc) {
	var name = env.dbname + "." + doc.create;
	var docStore, index, auto;

	if (docStore = Db.catalog[name]) {
		if (debug) print(" foundCollection: ", name);
	} else {
		if (debug) print(" newCollection: ", doc);
		docStore = jsdb_createDocStore(name, doc.size, doc.onDisk, &auto); 

		if (!doc.onDisk)
			Db.inMem.push(name);

		if (!docStore._id_ && doc.autoIndexId)
			if (index = jsdb_createIndex(docStore, {_id:1}, "_id_", "art", 0, true, false))
				docStore._id_ = index;

		Db.catalog[name] = docStore;
	}

	var result = { ok : 1.0 };

	jsdb_response(env.fileout, env.respid, env.id, 8, 0, env.replycode, 0, [ result]);
};

Db.insert = function(env, doc) {
	var docStore = Db._getCollection(env.dbname, doc.insert);
	var count, docId, result;

	if (env.replycode == 2011)
		doc = doc.documents;

	if (jsdb_insertDocs(docStore, doc, &docId, &count))
		if(debug) print("inserted: ", count); 

	if (env.replycode == 1)
		result = {
			writeErrors : [ ],
			writeConcernErrors : [ ],
			nInserted : count,
			nUpserted : 0,
			nMatched : 0,
			nRemoved : 0,
			upserted : 0,
			};
	else
		result = {
			ok : 1.0,
			n : count
			};

	jsdb_response(env.fileout, env.respid, env.id, 8, 0, env.replycode, 0, [ result]);
};

Db.find = function(env, doc) {
	var docStore = Db._getCollection(env.dbname, doc.find);
	var iterator, document, docId, out = 0, incl, cursor;
	var sort = doc.filter;
	var array = [];

	if (!sort) {
		iterator = jsdb_createIterator(docStore._docStore);

		while (docId = jsdb_nextDoc(iterator, &document))
			if (jsdb_findDocs(doc.filter, document))
				array[out++] = document;

	} else {
		cursor = jsdb_createCursor(docStore[sort.index], true, sort.start, sort.limit);

		while (docId = jsdb_nextKey(cursor, docStore._docStore, &document))
			if (jsdb_findDocs(doc.filter, document))
				array[out++] = document;
	}

	var result = {
		waitedMS : 10,
		cursor: {
			firstBatch : array, id : 0,
			ns : env.dbname + "." + doc[0].find,
			},
		ok : 1.0
		};

	jsdb_response (env.fileout, env.respid, env.id, 8, 0, env.replycode, [result]);
};

Db.createIndexes = function(env, doc) {
	var docStore = Db._getCollection(env.dbname, doc.createIndexes);
	var before = Object.keys(docStore).length - 1;
	var idx = 0, index, result, hndl;

	while (index = doc.indexes[idx++]) {
		if (hndl = jsdb_createIndex(docStore, index.key, index.name, index.type, index.size, index.unique, index.sparse, index.partialFilterExpression))
			docStore[index.name] = hndl;
		else
			print ("createIndex error: ", index);
	}

	var after = Object.keys(docStore).length - 1;

	result = {
		createdCollectionAutomatically : env.auto,
		numIndexesBefore : before,
		numIndexesAfter : after,
		ok : 1.0
	};
	
	jsdb_response(env.fileout, env.respid, env.id, 8, 0, env.replycode, [result]);
};

Db.count = function(env, doc) {
	var docStore = Db._getCollection(env.dbname, doc.count);
	var iterator, document, docId, count = 0, incl, cursor;
	var sort = doc.filter;

	iterator = jsdb_createIterator(docStore._docStore);

	while (docId = jsdb_nextDoc(iterator, &document))
		if (jsdb_findDocs(doc.filter, document))
			count++;

	var result = {
		n : count,
		ok : 1.0
	};

	jsdb_response (env.fileout, env.respid, env.id, 8, 0, env.replycode, [result]);
};

Db.replSetGetStatus = function(env, doc) {
	var result = {
		errmsg : "not running with --replSet",
		code : 76,
		ok : false
	};

	if(debug) print(result);

	if(debug) print("response ID:", env.respid, "  responding to:", env.id);
	jsdb_response(env.fileout, env.respid, env.id, 8, 0, env.replycode, 0, [ result ]);
};

Db.getLog = function(env, doc) {
	var result = {
		totalLinesWritten : 0,
		log : [],
		ok : 1.0
	};

	if(debug) print(result);

	if(debug) print("response ID:", env.respid, "  responding to:", env.id);
	jsdb_response(env.fileout, env.respid, env.id, 8, 0, env.replycode, 0, [result]);
};

Db.buildInfo = function(env, doc) {
	var result = {
		version : "3.3.1-269-gda2441b",
		targetMinOS : "Windows 7",
		allocator : "jsdb arena",
		javascriptEngine : "jsdb script",
		versionArray : [ 3, 3, 2, -100 ],
		bits : 64,
		debug : 1.0,
		ok : 1.0
	};

	if(debug) print(result);

	if(debug) print("response ID:", env.respid, "  responding to:", env.id);
	jsdb_response(env.fileout, env.respid, env.id, 8, 0, env.replycode, 0, [result]);
};

Db.whatsmyuri = function(env, doc) {
	var result = {
		you : "127.0.0.1:59000",
		ok : 1.0
	};

	if(debug) print(result);

	if(debug) print("response ID:", env.respid, "  responding to:", env.id);
	jsdb_response(env.fileout, env.respid, env.id, 8, 0, env.replycode, 0, [result]);
};

Db.isMaster = function(env, doc) {
	var result = {
		isMaster : 1,
		maxBsonObjectSize : 16777216,
		maxMessageSizeBytes : 48000000,
		maxWriteBatchSize : 1000,
		localTime : 12345678,
		maxWireVersion : 4,
		minWireVersion : 0,
		readOnly : 0,
		ok : 1.0
	};

	if(debug) print(result);

	if(debug) print("response ID:", env.respid, "  responding to:", env.id);
	jsdb_response(env.fileout, env.respid, env.id, 8, 0, env.replycode, 0, [result]);
};

Db.listCollections = function(env, doc) {
	var db, path, names;
	var ftw, result;
	var array = [];
	var idx = 0;

	ftw = jsdb_listFiles("data");

	for (path of Db.inMem)
		ftw.push(path);

	if(debug) print(ftw.sort());

	for (path of ftw.sort()) {
		names = path.split(".");
		if (names[0] == env.dbname)
			if (!idx || names[1] != array[idx])
				array[idx++] = {name : names[1]};
	}

	result = {
		cursor: {
			id : 0,
			ns : env.dbname + "." + "$cmd.listCollections",
			firstBatch : array
			},
		ok : 1.0
	};

	if(debug) print("response ID:", env.respid, "  responding to:", env.id);
	jsdb_response(env.fileout, env.respid, env.id, 8, 0, env.replycode, 0, [result]);
};

