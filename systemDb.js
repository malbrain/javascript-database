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

    idxKeyUnique = 10,	// index has unique key values
    idxKeySparse,
    idxKeyPartial,		// offset of partial filter doc
	idxBinary,			// treat string fields as binary
	idxType,			// HandleType

    btree1Bits = 20,    // Btree1 bits per page
	btree1Xtra,			// extra bits for leaves

	maxParam = 40		// maximum idx in use
};

var CursorOp = enum {
	opLeft		= 0x6c,		// 'l'
	opRight		= 0x72,		// 'r'
	opNext		= 0x6e,		// 'n'
	opPrev		= 0x70,		// 'p'
	opFind		= 0x66,		// 'f'
	opOne		= 0x6f,		// 'o'
	opBefore	= 0x62,		// 'b'
	opAfter		= 0x61		// 'a'
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
	return "DataBase " + this.name + "::" + this.options;
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

DocStore.prototype.createIndex = function(name, options, keySpec){
	return new Index(this, name, options, keySpec);
};

DocStore.prototype.createIterator = function(txnId, options){
	return new Iterator(this, txnId, options);
};

DocStore.prototype.toString = function() {
	return "DocStore " + this.name + "::" + this.options;
};

//	Index object

function Index(docStore, name, options, keySpec) {
	if (!this)
		return new Index(docStore, name, options, keySpec);

	var handle = jsdb_createIndex(docStore.valueOf(), name, DbOptParse(Index, options), keySpec);

	this.name = name;
	this.options = options;
	this.keySpec = keySpec;

	docStore[name] = handle;
	this.setValue(handle);
}

jsdb_installProps(Index, builtinProp.builtinIdx, _values.vt_index);

Index.prototype.toString = function() {
	return "Index " + this.name + "::" + this.options + "->" + this.keySpec;
};

Index.prototype.createCursor = function (txnId, options) {
	return new Cursor(this, txnId, options);
};

//	Cursor object

function Cursor(index, options, txnId) {
	if (!this)
		return new Cursor(index, options, txnId);

	var handle = jsdb_createCursor(index.valueOf(), txnId, DbOptParse(Cursor, options));

	this.index = index;
	this.options = options;
	this.setValue(handle);
}

jsdb_installProps(Cursor, builtinProp.builtinCursor, _values.vt_cursor);

Cursor.prototype.toString = function() {
	return "Cursor for " + this.index.name + "::" + this.options;
};

//	Iterator object

var IteratorPos = enum {
	PosBegin,
	PosEnd,
	PosAt
};

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
	var txt = "Iterator on " + this.docStore.name;

	if (this.options)
		txt += "::" + this.options;

	return txt;
};

//	Txn object

function Txn(db, options) {
	if (!this)
		return new Txn(db, options);

	var handle = jsdb_beginTxn(db, options);

	this.db = db;
	this.setValue(handle);
}

jsdb_installProps(Txn, builtinProp.builtinTxn, _values.vt_txn);

Txn.prototype.toString = function() {
	return "Transaction for " + this.db.name + "::" + this.options;
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
