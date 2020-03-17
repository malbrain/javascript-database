//  Options array slots

var HndlType = enum {
	Newarena = 0,
	AnyIndex,
	Catalog,
	Database,
	DocStore,
	ArtIndex,
	Btree1Index,
	Btree2Index,
	ColIndex,
	Iterator,
	Cursor,
	TXNs
};

var DbOptions = enum {
	size = 0,			// overall structure size
	onDisk,				// Arena resides on disk
	initSize,			// initial arena size
	objIdSize,			// internal use -- sizeof ObjId element
	clntSize,			// internal use -- sizeof ObjId element
	clntXtra,			// internal use -- sizeof ObjId element
	arenaXtra,			// internal use -- amt of xtra arena storage

	idxKeyUnique = 10,	// index has unique key values
	idxKeyDeferred,
	idxKeyAddr,			// compiled index key address
	idxKeySparse,
	idxKeyPartial,		// offset of partial filter doc
	idxKeyFlds,			// store field lengths in keys
	idxType,			// HandleType
	idxNoDocs,

	btree1Bits = 20,	// Btree1 bits per page
	btree1Xtra,			// extra bits for leaves

	btree2Bits = 22,	// Btree2 bits per page
	btree2Xtra,			// extra bits for leaves

	cursorDeDup = 25,	// de-duplicate cursor results

	userParams = 30,
	concurrency,		// concurrency mode enumeration

	maxParam = 63		// maximum idx in use
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

var TxnIsolation = enum {
	NotSpecified,
	SnapShot,
	ReadCommitted,
	Serializable
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

function Catalog(path, catname, isolation) {
	if (!this)
		return new Catalog(path, catname, isolation);

	this.db = jsdb_openCatalog(path, catname, isolation);

	this.path = path;
	this.name = catname;
	this.options = isolation;
}

jsdb_installProps(Catalog, builtinProp.builtinCatalog, _values.vt_catalog);

//	open the jsdb catalog

var catalog = new Catalog("dbdata", "Catalog", TxnIsolation.snapShot);

//	create an object holding docStores by name

function Db(dbname, options) {
	if (!this)
		return new Db(dbname, options);

	var opt = DbOptParse(Db, options);
	var handle = jsdb_openDatabase(dbname, opt);

	this.name = dbname;
	this.options = options;
	this.setValue(handle);

	catalog.db[dbname] = this;
}

jsdb_installProps(Db, builtinProp.builtinDb, _values.vt_db);

Db.prototype.toString = function() {
	return "DataBase " + this.name + "::" + this.options;
};

//	create a DocStore object holding indexes by name

Db.prototype.createDocStore = function(name, options) {
	return new DocStore(this, name, options);
};

Db.InitSize = 1024 * 1024;

//	create a DocStore object holding indexes by name

function DocStore(db, name, options) {
	if (!this)
		return new DocStore(db, name, options);

	var handle = jsdb_openDocStore(db, name, DbOptParse(DocStore, options));

	this.name = name;
	this.options = options;

	db[name] = handle;
	this.setValue(handle);
}

jsdb_installProps(DocStore, builtinProp.builtinStore, _values.vt_store);

DocStore.prototype.createIndex = function(name, options, keySpec){
	return new Index(this, name, options, keySpec);
};

DocStore.prototype.createIterator = function(options){
	return new Iterator(this, options);
};

DocStore.prototype.toString = function() {
	return "DocStore " + this.name + "::" + this.options;
};

//	Index object

function Index(docStore, name, options, keySpec) {
	if (!this)
		return new Index(docStore, name, options, keySpec);

	var handle = jsdb_createIndex(docStore, name, DbOptParse(Index, options), keySpec);

	this.name = name;
	this.options = options;
	this.keySpec = keySpec;
	this.docStore = docStore;

	docStore[name] = handle;
	this.setValue(handle);
}

//	default create index options

	Index.btree1Bits = 15;	// Btree1 bits per page
	Index.btree1Xtra = 5;	// extra bits for leaves

jsdb_installProps(Index, builtinProp.builtinIdx, _values.vt_index);

Index.prototype.toString = function() {
	return "Index " + this.name + "::" + this.options + "->" + this.keySpec;
};

Index.prototype.createCursor = function (options) {
	return new Cursor(this, options);
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

Cursor.prototype.next = function() {
	return this.move(CursorOp.opNext);
};

Cursor.prototype.prev = function() {
	return this.move(CursorOp.opPrev);
};

Cursor.prototype.toString = function() {
	return "Cursor for " + this.index.name + "::" + this.options;
};

//	Iterator operations

var IteratorOp = enum {
	opNext  = 0x6e,
	opPrev  = 0x70,
	opBegin = 0x62,
	opEnd   = 0x65,
	opSeek  = 0x73
};

function Iterator(docStore, options) {
	if (!this)
		return new Iterator(docStore, options);

	var handle = jsdb_createIterator(docStore, DbOptParse(Iterator, options));

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

function Txn(options) {
	if (!this)
		return new Txn(options);

	var txn = jsdb_beginTxn(DbOptParse(Txn, options));
	this.setValue(txn);

	this.read  = function(store, docIds) {
		return store.readDocs(docIds, txn);
	};

	this.update  = function(store, recs) {
		return store.updateDocs(recs, txn);
	};

	this.write = function(store, recs) {
		return store.writeDocs(recs, txn);
	};
}

Txn.prototype.commit = function(options) {
	this.commit(this, DbOptParse(Txn, options));
};

Txn.prototype.rollback = function(options) {
	this.rollback(this, DbOptParse(Txn, options));
};

var beginTxn = function(options) {
	return jsdb_beginTxn(DbOptParse(Txn, options));
};

jsdb_installProps(Txn, builtinProp.builtinTxn, _values.vt_txn);

//	DocId object

function DocId(docId) {
	if (!this)
		return new Doc(docId);

//	var doc = jsdb_makeDocument(docStore, docId);
	this.setValue(docId);
}

//	Document object

function Doc(doc) {
	if (!this)
		return new Doc(doc); // Store, docId);

//	var doc = jsdb_makeDocument(docStore, docId);
	this.setValue(doc);
}

//	Key object

function Key(v) {
	if (!this)
		return new Key(v);

	this.setValue(v);
}


jsdb_installProps(Doc, builtinProp.builtinDoc, _values.vt_document);
jsdb_installProps(DocId, builtinProp.builtinDocId, _values.vt_docId);
jsdb_installProps(Key, builtinProp.builtinKey, _values.vt_key);
