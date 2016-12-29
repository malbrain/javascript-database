//	create an object holding collections by name

function Db(dbname, options) {
	var handle = jsdb_openDatabase(dbname, options);

	this.dbname = dbname;
	this.setValue(handle);
}

jsdb_installProps(Db, builtinProp.builtinDb, _values.vt_db);

var db = new Db("test", {onDisk:true});

Db.prototype.toString = function() {
	return "DataBase \"" + this.dbname + "\"" + "[";
};

//	create a Collection object holding indexes by name

Db.prototype.create = function(name, options) {
	var handle = jsdb_openDocStore(this, name, options);

	this[name] = handle;
};

Db.defaultSize = 1024 * 1024;
Db.autoIndexId = true;
Db.onDisk = true;

//	create a Collection object holding indexes by name

function Collection(db, name, options) {
	var handle = jsdb_openDocStore(db, name, options);

	db[name] = handle;
	this.setValue(handle);
};

jsdb_installProps(Collection, builtinProp.builtinColl, _values.vt_coll);

//	Index object

function Index(collection, key, options) {
	var handle = jsdb_createIndex(collection, key, options);

	if (options.name)
		collection[options.name] = handle;

	this.setValue(handle);
};

jsdb_installProps(Index, builtinProp.builtinIdx, _values.vt_idx);

//	Cursor object

function Cursor(index, options) {
	var handle = jsdb_createCursor(index, options);

	this.setValue(handle);
};

jsdb_installProps(Cursor, builtinProp.builtinCursor, _values.vt_cursor);

//	Iterator object

function Iterator(collection, options) {
	var handle = jsdb_createIterator(collection, options);

	this.setValue(handle);
};

jsdb_installProps(Iterator, builtinProp.builtinIter, _values.vt_iter);

//	Txn object

function Txn(db) {
	var handle = jsdb_beginTxn(db);

	this.setValue(handle);
};

jsdb_installProps(Txn, builtinProp.builtinTxn, _values.vt_txn);
