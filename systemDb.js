//  Options array slots

var DbOptions = enum {
	OnDisk = 0,
	InitSize,
	UseTxn,
	NoDocs,
	DropDb,

	IdxKeySpec = 10,
	IdxKeySpecLen,		// this must immediately follow
    IdxKeyUnique,
    IdxKeySparse,
    IdxKeyPartial,
	IdxKeyPartialLen,	// this must immediately follow
	IdxType,			// 0 for artree, 1 & 2 for btree

    Btree1Bits = 20,    // Btree1 set
	Btree1Xtra,

	CursorTxn = 25,
	CursorStart,
	CursorEnd,

	MaxParam = 30
};

function DbOptParse(options) {
	var optVals = new Array(DbOptions.MaxParam);

	for (var name in options)
		optVals[DbOptions[name]] = options[name];

	return optVals;
}

//	create an object holding docStores by name

function Db(dbname, options) {
	var handle = jsdb_openDatabase(dbname, DbOptParse(options));

	this.name = dbname;
	this.options = options;
	this.setValue(handle);
}

jsdb_installProps(Db, builtinProp.builtinDb, _values.vt_db);

var db = new Db("test", {OnDisk:true});

Db.prototype.toString = function() {
	return "DataBase \"" + this.name + "\" " + this.options;
};

//	create a DocStore object holding indexes by name

Db.prototype.create = function(name, options) {
	var handle = jsdb_openDocStore(this, name, DbOptParse(options));

	this[name] = handle;
};

Db.DefaultSize = 1024 * 1024;
Db.AutoIndexId = true;
Db.OnDisk = true;

//	create a DocStore object holding indexes by name

function DocStore(db, name, options) {
	var handle = jsdb_openDocStore(db, name, DbOptParse(options));

	db[name] = handle;
	this.setValue(handle);
};

jsdb_installProps(DocStore, builtinProp.builtinStore, _values.vt_store);

//	Index object

DocStore.prototype.toString = function() {
	return "DataBase \"" + this.name + "\" " + this.options;
};

function Index(docStore, key, options) {
	var handle = jsdb_createIndex(docStore, key, DbOptParse(options));

	if (options.name)
		docStore[options.name] = handle;

	this.setValue(handle);
};

jsdb_installProps(Index, builtinProp.builtinIdx, _values.vt_idx);

Index.prototype.toString = function() {
	return "DataBase \"" + this.name + "\" " + this.options;
};

//	Cursor object

function Cursor(index, options) {
	var handle = jsdb_createCursor(index, DbOptParse(options));

	this.setValue(handle);
};

jsdb_installProps(Cursor, builtinProp.builtinCursor, _values.vt_cursor);

Cursor.prototype.toString = function() {
	return "DataBase \"" + this.name + "\" " + this.options;
};

//	Iterator object

function Iterator(docStore, options) {
	var handle = jsdb_createIterator(docStore, DbOptParse(options));

	this.setValue(handle);
};

jsdb_installProps(Iterator, builtinProp.builtinIter, _values.vt_iter);

Iterator.prototype.toString = function() {
	return "DataBase \"" + this.name + "\" " + this.options;
};

//	Txn object

function Txn(db) {
	var handle = jsdb_beginTxn(db);

	this.setValue(handle);
};

jsdb_installProps(Txn, builtinProp.builtinTxn, _values.vt_txn);

Txn.prototype.toString = function() {
	return "DataBase \"" + this.name + "\" " + this.options;
};

