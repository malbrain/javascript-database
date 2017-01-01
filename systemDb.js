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

function DbOptParse(base, options) {
	var optVals = new Array(DbOptions.MaxParam);

	// provide default values

	for (var name in DbOptions)
		optVals[DbOptions[name]] = base[name];

	for (var name in options)
		optVals[DbOptions[name]] = options[name];

	return optVals;
}

//	create an object holding docStores by name

function Db(dbname, options) {
	if (!this)
		return new Db(dbname, options);

	var handle = jsdb_openDatabase(dbname, DbOptParse(Db, options));

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

Db.prototype.createDocStore = function(name, options) {
	return new DocStore(this, name, options);
};

Db.prototype.beginTxn = function(options) {
	return new Txn(this, options);
};

Db.InitSize = 1024 * 1024;

//	create a DocStore object holding indexes by name

function DocStore(db, name, options) {
	if (!this)
		return new DocStore(db, name, options);

	var handle = jsdb_openDocStore(db, name, DbOptParse(options));

	this.name = name;
	this.options = options;

	db[name] = handle;
	this.setValue(handle);
};

jsdb_installProps(DocStore, builtinProp.builtinStore, _values.vt_store);

DocStore.prototype.createIndex = function(name, options){
	return new Index(this, name, options);
};

DocStore.prototype.toString = function() {
	return "DocStore \"" + this.name + "\" " + this.options;
};

//	Index object

function Index(docStore, name, options) {
	if (!this)
		return new Index(docStore, name, options);

	var handle = jsdb_createIndex(docStore, name, DbOptParse(options));

	this.name = name;
	this.options = options;

	docStore[name] = handle;
	this.setValue(handle);
};

jsdb_installProps(Index, builtinProp.builtinIdx, _values.vt_idx);

Index.prototype.toString = function() {
	return "DataBase \"" + this.name + "\" " + this.options;
};

//	Cursor object

function Cursor(index, options) {
	if (!this)
		return new Cursor(index, options);

	var handle = jsdb_createCursor(index, DbOptParse(options));

	this.index = index;
	this.options = options;
	this.setValue(handle);
};

jsdb_installProps(Cursor, builtinProp.builtinCursor, _values.vt_cursor);

Cursor.prototype.toString = function() {
	return "Cursor for \"" + this.index.toString() + "\" " + this.options;
};

//	Iterator object

function Iterator(docStore, options) {
	if (!this)
		return new Iterator(docStore, options);

	var handle = jsdb_createIterator(docStore, DbOptParse(options));

	this.docStore = docStore;
	this.options = options;
	this.setValue(handle);
};

jsdb_installProps(Iterator, builtinProp.builtinIter, _values.vt_iter);

Iterator.prototype.toString = function() {
	return "Iterator for \"" + this.docStore.toString() + "\" " + this.options;
};

//	Txn object

function Txn(db, options) {
	if (!this)
		return new Txn(db, options);

	var handle = jsdb_beginTxn(db, options);

	this.setValue(handle);
};

jsdb_installProps(Txn, builtinProp.builtinTxn, _values.vt_txn);

Txn.prototype.toString = function() {
	return "DataBase \"" + this.name + "\" " + this.options;
};

