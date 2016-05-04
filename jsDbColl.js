//  Database collection implementation

var DbCollection = function(fullName, options) {
	this._fullName = fullName;
	this._options = options;
};

DbCollection.prototype._install = function() {
	var created;

	this._docStore = jsdb_createDocStore(this._fullName, this._options.size, this._options.onDisk, &created);

	if (!this._options.onDisk)
		Db.inMem.push(this._fullName);

	if (created)
	 if (!this._docStore._id_ && this._options.autoIndexId)
	  this._docStore._id_ = jsdb_createIndex(this._docStore, {_id:1}, "_id_", "art", 0, true, false);

	return Db.catalog[this._fullName] = this._docStore;
};

Db.prototype.getCollection = function(name) {
	var fullName = this._dbname + "." + name;
	return this[name] = new DbCollection(fullName, {size : Db._defaultSize, onDisk : Db._onDisk, autoIndexId : Db._autoIndexId}); 
};

Db.prototype.createCollection = function(name, options) {
	var fullName = this._dbname + "." + name;

	return this[name] = new DbCollection(fullName, options);
};

DbCollection.prototype.save = function (document, concern) {
	var docId, count;

	if (!this._docStore)
		this._install();

	if (jsdb_insertDocs(this._docStore, document, &docId, &count))
		return { nInserted : count};

	return {
		nInserted : 0,
		writeConcernError : {
			code : 64,
			errmsg : "timeout"
		}
	};
				
};

DbCollection.prototype.createIndex = function(key, options) {
	var hndl, prev;
	
	if (!this._docStore)
		this._install();

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
