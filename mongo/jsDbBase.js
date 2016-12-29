var debug = true;

var Db = function(dbname) {
	this._dbname = dbname;
	this._db = jsdb_openDatabase(dbname, Db._onDisk);
};

Db.prototype.beginTxn = function() {
	this._dbtxn = jsdb_beginTxn(this._handle);
};

Db.prototype.commitTxn = function() {
	jsdb_commitTxn(this._handle, this._dbtxn);
	this._dbtxn = null;
};

Db.prototype.rollbackTxn = function() {
	jsdb_rollbackTxn(this._handle, this._dbtxn);
	this._dbtxn = null;
};

Db.catalog = {};
Db.inMem = [];

Db._defaultSize = 1024 * 1024;
Db._autoIndexId = true;
Db._onDisk = true;
