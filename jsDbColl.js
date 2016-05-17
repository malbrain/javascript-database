//  Database collection implementation

var DbCollection = function(database, name, options) {
	this._database = database;
	this._options = options;
	var created;

	this._docStore = jsdb_createDocStore(database, name, options.size, options.onDisk, &created);

	if (!options.onDisk)
		Db.inMem.push(name);

	if (created)
	 if (!this._docStore._id_ && options.autoIndexId)
	  this._docStore._id_ = jsdb_createIndex(this._docStore, {_id:1}, "_id_", "art", 0, true, false);

	Db.catalog[name] = this._docStore;
};

Db.prototype.getCollection = function(name) {
	this[name] = new DbCollection(this._db, name, {size : Db._defaultSize, onDisk : Db._onDisk, autoIndexId : Db._autoIndexId}); 
};

Db.prototype.createCollection = function(name, options) {
	return this[name] = new DbCollection(this._db, name, options);
};

DbCollection.prototype.save = function (document, concern) {
	var docId, count, dbtxn = this._dbtxn, result;

	if (!dbtxn)
		dbtxn = jsdb_beginTxn(this._database);

	if (jsdb_insertDocs(this._docStore, document, &docId, &count, dbtxn)) {
		result = { nInserted : count};
		if (!this._dbtxn)
			jsdb_commitTxn(this._database, dbtxn);
	} else { 
		result = {
		  nInserted : 0,
		  writeConcernError : {
			code : 64,
			errmsg : "timeout"
		  }
		};
		if (!this._dbtxn)
			jsdb_rollbackTxn(this._database, dbtxn);
	};
};

DbCollection.prototype.createIndex = function(key, options) {
	var hndl, prev;
	
	prev = Object.keys(this._docStore).length - 1;

	if (hndl = jsdb_createIndex(this._docStore, key, options.name, options.type, options.size, options.unique, options.sparse, options.partialFilterExpression))

		this._docStore[options.name] = hndl;
	else
		print ("createIndex error: ", options.name);

	return {
		numIndexesBefore : prev,
		numIndexesAfter: Object.keys(this._docStore).length - 1,
		ok : true
	};
};
