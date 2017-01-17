//  Options array slots

var HndlType = enum {
	Newarena = 0,
	Catalog,
	Database,
	DocStore,
	ArtIndex,
	Btree1Index,
	Btree2Index,
	ColIndex,
	Iterator,
	Cursor,
	Txn
};

var DbOptions = enum {
	size = 0,			// overall structure size
	onDisk,				// Arena resides on disk
	initSize,			// initial arena size
	useTxn,				// use transactions
	noDocs,				// indexes only
	dropDb,

	idxKeySpec = 10,	// offset of key spec document
    idxKeyUnique,		// index has unique key values
    idxKeySparse,
    idxKeyPartial,		// offset of partial filter doc
	idxBinary,			// treat string fields as binary
	idxType,			// HandleType

    btree1Bits = 20,    // Btree1 bits per page
	btree1Xtra,			// extra bits for leaves

	cursorTxn = 25,
	cursorStart,
	cursorEnd,

	maxParam = 40		// maximum idx in use
};

function DbOptParse(base, options) {
	var optVals = new Array(DbOptions.maxParam + 1);

	// provide default values

	for (var name in DbOptions)
		optVals[DbOptions[name]] = base[name];

	if (options)
	  for (var name in options)
		optVals[DbOptions[name]] = options[name];

	return optVals;
}

//	create an object holding docStores by name

function Db(dbname, options) {
	if (!this)
		return new Db(dbname, options);

	var opt = DbOptParse(Db, options);
	var handle = jsdb_openDatabase(dbname, opt);

	this.name = dbname;
	this.options = options;
	this.setValue(handle);
}

jsdb_installProps(Db, builtinProp.builtinDb, _values.vt_db);

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

	var handle = jsdb_openDocStore(db.valueOf(), name, DbOptParse(DocStore, options));

	this.name = name;
	this.options = options;

	db[name] = handle;
	this.setValue(handle);
}

jsdb_installProps(DocStore, builtinProp.builtinStore, _values.vt_store);

DocStore.prototype.createIndex = function(name, options){
	return new Index(this, name, options);
};

DocStore.prototype.createIterator = function(txnId, options){
	return new Iterator(this, txnId, options);
};

DocStore.prototype.toString = function() {
	return "DocStore \"" + this.name + "\" " + this.options;
};

//	Index object

function Index(docStore, name, options) {
	if (!this)
		return new Index(docStore, name, options);

	var handle = jsdb_createIndex(docStore, name, DbOptParse(Index, options));

	this.name = name;
	this.options = options;

	docStore[name] = handle;
	this.setValue(handle);
}

jsdb_installProps(Index, builtinProp.builtinIdx, _values.vt_idx);

Index.prototype.toString = function() {
	return "DataBase \"" + this.name + "\" " + this.options;
};

//	Cursor object

function Cursor(index, options) {
	if (!this)
		return new Cursor(index, options);

	var handle = jsdb_createCursor(index, DbOptParse(Cursor, options));

	this.index = index;
	this.options = options;
	this.setValue(handle);
}

jsdb_installProps(Cursor, builtinProp.builtinCursor, _values.vt_cursor);

Cursor.prototype.toString = function() {
	return "Cursor for \"" + this.index.toString() + "\" " + this.options;
};

//	Iterator object

function Iterator(docStore, txnId, options) {
	if (!this)
		return new Iterator(docStore, txnId, options);

	var handle = jsdb_createIterator(docStore.valueOf(), txnId, DbOptParse(Iterator, options));

	this.docStore = docStore;
	this.options = options;
	this.setValue(handle);
}

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
}

jsdb_installProps(Txn, builtinProp.builtinTxn, _values.vt_txn);

Txn.prototype.toString = function() {
	return "DataBase \"" + this.name + "\" " + this.options;
};

//	Document object

function Doc(docStore, docId) {
}

//	DocId object

function DocId(v) {
	if (this)
		this.setValue(v.toString());
	else
		return v.toString();
}

jsdb_installProps(Doc, builtinProp.builtinDoc, _values.vt_document);
jsdb_installProps(DocId, builtinProp.builtinDocId, _values.vt_docId);
