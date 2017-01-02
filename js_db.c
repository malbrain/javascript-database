#include "js.h"
#include "js_props.h"
#include "js_dbindex.h"
#include "database/db.h"
#include "database/db_api.h"
#include "database/db_map.h"
#include "database/db_object.h"
#include "database/db_handle.h"
#include "database/db_arena.h"

void marshal_doc(value_t document, uint8_t *doc, uint32_t offset, uint32_t docSize);
uint32_t calcSize (value_t doc);

void js_deleteHandle(value_t val) {
	deleteHandle((DbHandle *)&val.handle);
}

uint32_t sizeOption(value_t val) {
	if (val.type == vt_string) {
		string_t *str = js_addr(val);
		return str->len + sizeof(ParamVal);
	}

	if (val.type == vt_object)
		return calcSize(val) + sizeof(ParamVal);

	return 0;
}

uint64_t processOptions(value_t options) {
	array_t *aval = js_addr(options);
	value_t *values = options.marshaled ? aval->valueArray : aval->valuePtr;
	uint32_t cnt = options.marshaled ? aval->cnt : vec_cnt(aval->valuePtr);
	uint32_t offsets[MaxParam];
	uint32_t sizes[MaxParam];
	ParamVal *paramVal;
	Params *params;
	uint64_t bits;

	memset (offsets, 0, sizeof(offsets));
	memset (sizes, 0, sizeof(sizes));

	sizes[Size] = sizeof(Params);

	offsets[IdxKeySpec] = sizes[Size];
	sizes[Size] += sizeOption(values[IdxKeySpec]);

	offsets[IdxKeyPartial] = sizes[Size];
	sizes[Size] += sizeOption(values[IdxKeyPartial]);

	offsets[CursorStart] = sizes[Size];
	sizes[Size] += sizeOption(values[CursorStart]);

	offsets[CursorEnd] = sizes[Size];
	sizes[Size] += sizeOption(values[CursorEnd]);

	if ((bits = db_rawAlloc(sizes[Size], true)))
		params = db_memObj(bits);
	else {
		fprintf (stderr, "processOptions: out of memory!\n");
		exit(1);
	}

	params[Size].intVal = sizes[Size];

	//	process the passed params array

	for (int idx = 0; idx < cnt; idx++) {
	  switch (idx) {
		case OnDisk:
			params[idx].boolVal = conv2Bool(values[idx], false).boolean;
			break;

		case InitSize:
			params[idx].intVal = conv2Int(values[idx], false).nval;
			break;

		case UseTxn:
			params[idx].boolVal = conv2Bool(values[idx], false).boolean;
			break;

		case NoDocs:
			params[idx].boolVal = conv2Bool(values[idx], false).boolean;
			break;

		case DropDb:
			params[idx].boolVal = conv2Bool(values[idx], false).boolean;
			break;

		case IdxKeyPartial:
		case IdxKeySpec:
			marshal_doc(values[idx], (uint8_t *)params, offsets[idx] + sizeof(ParamVal), sizes[idx] - sizeof(ParamVal));

			params[idx].offset = offsets[idx];
			paramVal = (ParamVal *)((uint8_t *)params + offsets[idx]);
			paramVal->len = sizes[idx] - sizeof(ParamVal);
			break;

		case IdxKeyUnique:
			params[idx].boolVal = conv2Bool(values[idx], false).boolean;
			break;

		case IdxKeySparse:
			params[idx].boolVal = conv2Int(values[idx], false).boolean;
			break;

		case Btree1Bits:
			params[idx].intVal = conv2Int(values[idx], false).nval;
			break;

		case Btree1Xtra:
			params[idx].intVal = conv2Int(values[idx], false).nval;
			break;

		case CursorTxn:
			params[idx].intVal = conv2Int(values[idx], false).nval;
			break;

		case CursorStart:
		case CursorEnd: {
			if (values[idx].type == vt_string) {
				string_t *str = js_addr(values[idx]);
				params[idx].offset = offsets[idx];
				paramVal = (ParamVal *)((uint8_t *)params + offsets[idx]);
				paramVal->len = sizes[idx] - sizeof(ParamVal);
				memcpy(paramVal->val, str->val, paramVal->len);
			}

			break;
		}
	  }
	}

	return bits;
}

value_t fcnCollInsert(value_t *args, value_t *thisVal) {
	object_t *oval = js_addr(*thisVal);
	value_t v, s, resp = newArray(array_value);
	array_t *respval = resp.addr;
	Doc *doc;

	s.bits = vt_status;

	if (args[0].type == vt_array) {
	  array_t *aval = js_addr(args[0]);
	  value_t *values = args[0].marshaled ? aval->valueArray : aval->valuePtr;
	  uint32_t cnt = args[0].marshaled ? aval->cnt : vec_cnt(aval->valuePtr);

	  for (int idx = 0; idx < cnt; idx++) {
		int size = calcSize(values[idx]);

		if ((s.status = allocDoc((DbHandle *)oval->base->handle, &doc, 0)))
			return s;

		marshal_doc(values[idx], (uint8_t*)doc, sizeof(Doc), size);
		  
		if ((s.status = assignDoc((DbHandle *)oval->base->handle, doc, 0)))
			return s;

		v.bits = vt_docId;
		v.docBits = doc->ver->docId.bits;
		vec_push(respval->valuePtr, v);
	  }

	}

	return resp;
}

PropFcn builtinDbFcns[] = {
//	{ fcnDbOnDisk, "onDisk" },
	{ NULL, NULL}
};

PropVal builtinDbProp[] = {
//	{ propDbOnDisk, "onDisk" },
	{ NULL, NULL}
};

PropFcn builtinCollFcns[] = {
	{ fcnCollInsert, "insert" },
	{ NULL, NULL}
};

PropVal builtinCollProp[] = {
//	{ propCollOnDisk, "onDisk" },
	{ NULL, NULL}
};

PropFcn builtinIterFcns[] = {
//	{ fcnIterOnDisk, "onDisk" },
	{ NULL, NULL}
};

PropVal builtinIterProp[] = {
//	{ propIterOnDisk, "onDisk" },
	{ NULL, NULL}
};

PropVal builtinIdxFcns[] = {
//	{ fcnIdxOnDisk, "onDisk" },
	{ NULL, NULL}
};

PropVal builtinIdxProp[] = {
//	{ propIdxOnDisk, "onDisk" },
	{ NULL, NULL}
};

PropVal builtinCursorFcns[] = {
//	{ fcnIdxOnDisk, "onDisk" },
	{ NULL, NULL}
};

PropVal builtinCursorProp[] = {
//	{ propIdxOnDisk, "onDisk" },
	{ NULL, NULL}
};

PropFcn builtinTxnFcns[] = {
//	{ fcnTxnBeginTxn, "beginTxn" },
	{ NULL, NULL}
};

PropFcn builtinTxnProp[] = {
//	{ propTxnBeginTxn, "beginTxn" },
	{ NULL, NULL}
};

//	openDatabase(name, options)

value_t js_openDatabase(uint32_t args, environment_t *env) {
	value_t v, opts, name;
	string_t *namestr;
	Params *params;
	DbHandle db[1];
	uint64_t bits;
	value_t s;

	s.bits = vt_status;

	if (debug) fprintf(stderr, "funcall : openDatabase\n");

	name = eval_arg (&args, env);

	if (name.type == vt_string)
		namestr = js_addr(name);
	else {
		fprintf(stderr, "Error: openDatabase => expecting dbname:string => %s\n", strtype(name.type));
		return s.status = ERROR_script_internal, s;
	}

	// process options array

	opts = eval_arg (&args, env);

	if (opts.type == vt_array)
		bits = processOptions(opts);
	else {
		fprintf(stderr, "Error: openDatabase => expecting options:array => %s\n", strtype(opts.type));
		return s.status = ERROR_script_internal, s;
	}

	abandonValue(opts);
	params = db_memObj(bits);

	if ((s.status = (int)openDatabase(db, namestr->val, namestr->len, params)))
		return s;

	v.bits = vt_db;
	v.subType = Hndl_database;
	*v.handle = db->hndlBits;

	db_memFree(bits);
	return v;
}

//  createIndex(docStore, options)

value_t js_createIndex(uint32_t args, environment_t *env) {
	value_t docStore, opts, name;
	string_t *namestr;
	DbHandle idx[1];
	Params *params;
	uint64_t bits;
	value_t s;

	s.bits = vt_status;

	if (debug) fprintf(stderr, "funcall : CreateIndex\n");

	docStore = eval_arg (&args, env);

	if (vt_store != docStore.type || Hndl_docStore != docStore.subType) {
		fprintf(stderr, "Error: createIndex => expecting docStore:handle => %s\n", strtype(docStore.type));
		return s.status = ERROR_script_internal, s;
	}

	name = eval_arg (&args, env);

	if (name.type == vt_string)
		namestr = js_addr(name);
	else {
		fprintf(stderr, "Error: createIndex => expecting name:string => %s\n", strtype(name.type));
		return s.status = ERROR_script_internal, s;
	}

	// process options array

	opts = eval_arg (&args, env);

	if (opts.type == vt_array)
		bits = processOptions(opts);
	else {
		fprintf(stderr, "Error: createIndex => expecting options:array => %s\n", strtype(opts.type));
		return s.status = ERROR_script_internal, s;
	}

	abandonValue(opts);
	params = db_memObj(bits);

	if ((s.status = (int)createIndex(idx, (DbHandle *)docStore.handle, namestr->val, namestr->len, params)))
		return s;

	s.bits = vt_index;
	s.subType = Hndl_artIndex + params[IdxType].intVal;
	*s.handle = idx->hndlBits;

	db_memFree(bits);
	return s;
}

//  createCursor(index, options)

value_t js_createCursor(uint32_t args, environment_t *env) {
	value_t index, opts;
	DbHandle cursor[1];
	Params *params;
	uint64_t bits;
	value_t s;

	s.bits = vt_status;

	if (debug) fprintf(stderr, "funcall : createCursor\n");

	index = eval_arg (&args, env);

	if (vt_index != index.type || (Hndl_btree1Index != index.subType && Hndl_artIndex != index.subType)) {
		fprintf(stderr, "Error: createCursor => expecting index:handle => %s\n", strtype(index.type));
		return s.status = ERROR_script_internal, s;
	}

	// process options array

	opts = eval_arg (&args, env);

	if (opts.type == vt_array)
		bits = processOptions(opts);
	else {
		fprintf(stderr, "Error: createCursor => expecting options:array => %s\n", strtype(opts.type));
		return s.status = ERROR_script_internal, s;
	}

	abandonValue(opts);
	params = db_memObj(bits);

	if ((s.status = (int)createCursor(cursor, (DbHandle *)index.handle, params)))
		return s;

	s.bits = vt_cursor;
	s.subType = Hndl_cursor;
	*s.handle = cursor->hndlBits;

	db_memFree(bits);
	return s;
}

//	openDocStore(database, options)

value_t js_openDocStore(uint32_t args, environment_t *env) {
	value_t database, opts, name;
	DbHandle docStore[1];
	string_t *namestr;
	Params *params;
	uint64_t bits;
	value_t s;

	s.bits = vt_status;

	if (debug) fprintf(stderr, "funcall : openDocStore\n");

	database = eval_arg (&args, env);

	if (vt_db != database.type || Hndl_database != database.subType) {
		fprintf(stderr, "Error: openDocStore => expecting Database handle => %s\n", strtype(database.type));
		return s.status = ERROR_script_internal, s;
	}

	name = eval_arg (&args, env);

	if (name.type == vt_string)
		namestr = js_addr(name);
	else {
		fprintf(stderr, "Error: openDocStore => expecting name:string => %s\n", strtype(name.type));
		return s.status = ERROR_script_internal, s;
	}

	// process options array

	opts = eval_arg (&args, env);

	if (opts.type == vt_array)
		bits = processOptions(opts);
	else {
		fprintf(stderr, "Error: openDocStore => expecting options:array => %s\n", strtype(opts.type));
		return s.status = ERROR_script_internal, s;
	}

	abandonValue(opts);
	params = db_memObj(bits);

	if ((s.status = (int)openDocStore(docStore, (DbHandle *)database.handle, namestr->val, namestr->len, params)))
		return s;

	abandonValue(opts);

	s.bits = vt_store;
	s.subType = Hndl_docStore;
	*s.handle = docStore->hndlBits;

	db_memFree(bits);
	return s;
}

//  js_createIterator(docStore, options)

value_t js_createIterator(uint32_t args, environment_t *env) {
	value_t docStore, opts;
	DbHandle iter[1];
	Params *params;
	uint64_t bits;
	value_t s;

	s.bits = vt_status;

	if (debug) fprintf(stderr, "funcall : CreateIterator\n");

	docStore = eval_arg (&args, env);

	if (vt_store != docStore.type || Hndl_docStore != docStore.subType) {
		fprintf(stderr, "Error: createIterator => expecting docStore:Handle => %s\n", strtype(docStore.type));
		return s.status = ERROR_script_internal, s;
	}

	// process options array

	opts = eval_arg (&args, env);

	if (opts.type == vt_array)
		bits = processOptions(opts);
	else {
		fprintf(stderr, "Error: createIterator => expecting options:array => %s\n", strtype(opts.type));
		return s.status = ERROR_script_internal, s;
	}

	abandonValue(opts);
	params = db_memObj(bits);

	if ((s.status = (int)createIterator(iter, (DbHandle *)docStore.handle, params)))
		return s;

	s.bits = vt_iter;
	s.subType = Hndl_iterator;
	*s.handle = iter->hndlBits;

	db_memFree(bits);
	return s;
}

//	beginTxn(db, options)

value_t js_beginTxn(uint32_t args, environment_t *env) {
	value_t db, txnId, opts;
	Params *params;
	uint64_t bits;
	value_t s;

	s.bits = vt_status;

	if (debug) fprintf(stderr, "funcall : beginTxn\n");

	db = eval_arg (&args, env);

	if (vt_db != db.type || Hndl_database != db.subType) {
		fprintf(stderr, "Error: beginTxn => expecting Db:handle => %s\n", strtype(db.type));
		return s.status = ERROR_script_internal, s;
	}

	// process options array

	opts = eval_arg (&args, env);

	if (opts.type == vt_array)
		bits = processOptions(opts);
	else {
		fprintf(stderr, "Error: createIterator => expecting options:array => %s\n", strtype(opts.type));
		return s.status = ERROR_script_internal, s;
	}

	abandonValue(opts);
	params = db_memObj(bits);

	txnId.bits = vt_txnId;

	if((txnId.txnBits = beginTxn((DbHandle *)db.handle, params)))
		return txnId;
	else
		s.status = ERROR_outofmemory;

	return s;
}

//	commitTxn(db, txnId)

value_t js_commitTxn(uint32_t args, environment_t *env) {
	value_t db, txnId;
	value_t s;

	s.bits = vt_status;

	if (debug) fprintf(stderr, "funcall : commitTxn\n");

	db = eval_arg (&args, env);

	if (vt_db != db.type || Hndl_database != db.subType) {
		fprintf(stderr, "Error: commitTxn => expecting Db:handle => %s\n", strtype(db.type));
		return s.status = ERROR_script_internal, s;
	}

	txnId = eval_arg (&args, env);

	if (vt_txnId != txnId.type) {
		fprintf(stderr, "Error: commitTxn => expecting Db:txnId => %s\n", strtype(txnId.type));
		return s.status = ERROR_script_internal, s;
	}

	s.status = (int)commitTxn((DbHandle*)db.handle, txnId.txnBits);
	return s;
}

//	rollbackTxn(db, txnId)

value_t js_rollbackTxn(uint32_t args, environment_t *env) {
	value_t db, txnId;
	value_t s;

	s.bits = vt_status;

	if (debug) fprintf(stderr, "funcall : rollbackTxn\n");

	db = eval_arg (&args, env);

	if (vt_db != db.type || Hndl_database != db.subType) {
		fprintf(stderr, "Error: rollbackTxn => expecting Db:handle => %s\n", strtype(db.type));
		return s.status = ERROR_script_internal, s;
	}

	txnId = eval_arg (&args, env);

	if (vt_txnId != txnId.type) {
		fprintf(stderr, "Error: rollbackTxn => expecting Db:txnId => %s\n", strtype(txnId.type));
		return s.status = ERROR_script_internal, s;
	}

	s.status = (int)rollbackTxn((DbHandle *)db.handle, txnId.txnBits);
	return s;
}

