#include "js.h"
#include "js_db.h"
#include "js_dbindex.h"
#include "js_props.h"

extern value_t newDate(value_t *args);

//	vector of handles to all docStore for all db

extern char hndlInit[1];
extern DbMap *hndlMap;
extern Catalog *catalog;
// extern CcMethod *cc;

JsDoc *docAddr(struct Document *base) {
	if (base->docType == VerMvcc) {
		Doc *doc = (Doc *)base;
		return (JsDoc *)(doc->doc->base + doc->newestVer + sizeof(Ver));
	}

	return (JsDoc *)(base->base + sizeof(struct Document));
}

Doc *mvccDoc(struct Document *doc) {
  return (Doc *)(doc->base);
}

Ver *mvccVer(value_t val) { 
	return (Ver *)(val.document->base + val.offset); 
}

JsStatus badHandle(value_t hndl) { return (JsStatus)DB_ERROR_badhandle; }

Handle *js_handle(value_t hndl, int hndlType) {

  while (hndl.type == vt_object)
	  hndl = *hndl.oval->baseVal;

  if (hndl.type == vt_hndl)
    if (hndl.subType == hndlType)
      return bindHandle(hndl.hndl, hndlType);
    else if (hndlType == Hndl_anyIdx)
      if (hndl.subType == Hndl_btree1Index ||
          hndl.subType == Hndl_btree2Index || hndl.subType == Hndl_artIndex)
        return bindHandle(hndl.hndl, hndlType);

  return NULL;
}

void processOptions(Params *params, value_t options) {
	uint32_t size = sizeof(Params) * (MaxParam + 1);
	dbarray_t *dbaval = js_dbaddr(options, NULL);
	uint32_t cnt, idx;
	value_t *values;

	memset (params, 0, size);
	params[Size].intVal = size;

	if (options.type == vt_endlist)
		return;

	if (options.type != vt_array) {
		fprintf(stderr, "Error: processOptions => expecting options:array => %s\n", strtype(options));
		return;
	}

	values = options.marshaled ? dbaval->valueArray : options.aval->valuePtr;
	cnt = options.marshaled ? dbaval->cnt : vec_cnt(values);

	if (cnt > MaxParam + 1)
		cnt = MaxParam + 1;

	//	process the passed params array

	for (idx = 0; idx < cnt; idx++) {
	  switch (idx) {
		case OnDisk:
			params[idx].boolVal = conv2Bool(values[idx], false).boolean;
			break;

		case RecordType:
			params[idx].intVal = conv2Int(values[idx], false).nval;
			break;

		case InitSize:
			params[idx].intVal = conv2Int(values[idx], false).nval;
			break;

		case IdxKeyUnique:
			params[idx].boolVal = conv2Bool(values[idx], false).boolean;
			break;

		case IdxKeyFlds:
			params[idx].boolVal = conv2Bool(values[idx], false).boolean;
			break;

		case IdxType:
			params[idx].intVal += conv2Int(values[idx], false).nval;
			break;

		case IdxKeySparse:
			params[idx].boolVal = conv2Bool(values[idx], false).boolean;
			break;

		case Btree1Bits:
			params[idx].intVal = conv2Int(values[idx], false).nval;
			break;

		case Btree1Xtra:
			params[idx].intVal = conv2Int(values[idx], false).nval;
			break;

		case CursorDeDup:
			params[idx].boolVal = conv2Bool(values[idx], false).boolean;
			break;

		case Concurrency:
			params[idx].intVal = conv2Int(values[idx], false).nval;
			break;
	  }
	}
}

//	closeHandle (handle)

value_t js_closeHandle(uint32_t args, environment_t *env) {
	value_t hndl, s;

	s.bits = vt_status;
	hndl = eval_arg (&args, env);

	if (hndl.type != vt_hndl) {
      fprintf(stderr, "Error: closeHandle => expecting Handle type => %s\n",
              strtype(hndl));

	  return s.status = ERROR_script_internal, s;
	}

	return s.status = closeHandle(hndl.hndl), s;
}

//	openCatalog(path, name, txnisolation)

extern int numDocStores;

value_t js_openCatalog(uint32_t args, environment_t *env) {
	value_t s, path, name, v, dbs;
	string_t *namestr, *pathstr;
	PathStk pathStk[1];
	RedBlack *entry;

	s.bits = vt_status;

	if (debug) fprintf(stderr, "funcall : openCatalog\n");

	path = eval_arg(&args, env);

	if (vt_string == path.type)
		pathstr = js_dbaddr(path, NULL);
	else {
		fprintf(stderr, "Error: openCatalog => expecting path:string => %s\n", strtype(path));
		return s.status = ERROR_script_internal, s;
	}

	name = eval_arg(&args, env);

	if (vt_string == name.type)
		namestr = js_dbaddr(name, NULL);
	else {
		fprintf(stderr, "Error: openCatalog => expecting name:string => %s\n", strtype(name));
		return s.status = ERROR_script_internal, s;
	}

	if (!*hndlInit)
          initHndlMap((char *)pathstr->val, pathstr->len,
                      (char *
						  )namestr->val, true, numDocStores);

	catalog = (Catalog *)(hndlMap->arena + 1);
//	cc = (CcMethod *)(catalog + 1);

	v = eval_arg(&args, env);
/*
	if (v.type == vt_int)
		cc->isolation = v.nval;
*/
	abandonValue(name);
	abandonValue(path);

	dbs = newObject(vt_object);

	lockLatch(hndlMap->arenaDef->nameTree->latch);

	if ((entry = rbStart (hndlMap, pathStk, hndlMap->arenaDef->nameTree)))
	  do {
		ArenaDef *arenaDef = (ArenaDef *)(entry + 1);
		if (*arenaDef->dead & KILL_BIT)
			continue;
		value_t dbname = newString(rbkey(entry), entry->keyLen);
		v = lookup(dbs, dbname, true, 0);
		v.lval->bits = vt_date;
		v.lval->date = arenaDef->creation * 1000ULL;
	  } while ((entry = rbNext(hndlMap, pathStk)));

	unlockLatch(hndlMap->arenaDef->nameTree->latch);
	return dbs;
}

//	openDatabase(name, options)

value_t js_openDatabase(uint32_t args, environment_t *env) {
	Params params[MaxParam + 1];
	value_t opts, name;
	string_t *namestr;
	value_t dbHndl;
	value_t s;

	s.bits = vt_status;

	if (debug) fprintf(stderr, "funcall : openDatabase\n");

	name = eval_arg (&args, env);

	if (name.type == vt_string)
		namestr = js_dbaddr(name, NULL);
	else {
		fprintf(stderr, "Error: openDatabase => expecting dbname:string => %s\n", strtype(name));
		return s.status = ERROR_script_internal, s;
	}

	// process options array

	opts = eval_arg (&args, env);
	processOptions(params, opts);
	abandonValue(opts);

	if ((s.status = (int)openDatabase(dbHndl.hndl, (char *)namestr->val, namestr->len, params)))
		return s;

	dbHndl.bits = vt_hndl;
	dbHndl.subType = Hndl_database;

	abandonValue(name);
	return dbHndl;
}

//  createIndex(docStore, name, options , keySpec)

value_t js_createIndex(uint32_t args, environment_t *env) {
	DbHandle idxDbHndl[1];
  value_t store, opts, name, spec;
	Params params[MaxParam + 1];
    DbMap *docMap, *idxMap;
    DbIndex *index;
	string_t *namestr;
	Handle *docHndl;
    Handle *idxHndl;
	value_t s;

	s.bits = vt_status;

	if (debug) fprintf(stderr, "funcall : CreateIndex\n");

	store = eval_arg (&args, env);

	if(!(docHndl = js_handle(store, Hndl_docStore)))
		return s.status = DB_ERROR_handleclosed, s;
    else
        docMap = MapAddr(docHndl);

	name = eval_arg (&args, env);

	if (name.type == vt_string)
		namestr = js_dbaddr(name, NULL);
	else {
		fprintf(stderr, "Error: createIndex => expecting name:string => %s\n", strtype(name));
		return s.status = ERROR_script_internal, s;
	}

	// process options array

	opts = eval_arg (&args, env);
	processOptions(params, opts);
	abandonValue(opts);

	//  create the index arena

	if ((s.status = (int)createIndex(idxDbHndl, docHndl->hndl, (char *)namestr->val, namestr->len, params)))
		return s;

    if (!(idxHndl = bindHandle(idxDbHndl, Hndl_anyIdx)))
      return s.status = DB_ERROR_handleclosed, s;
    else
      idxMap = MapAddr(idxHndl);

	index = (DbIndex *)(idxMap->arena + 1);

	//	process keyspec object

    spec = eval_arg(&args, env);

    if (spec.type == vt_object)
      index->keySpec = compileKey(idxHndl, spec);

    abandonValue(spec);

    //  install the new index in the docStore

//	idx = arrayAlloc(mapAddr(docHndl), docIdx->idxHndls, sizeof(DbHandle));
//	idxHndlAddr = arrayEntry(mapAddr(docHndl), docIdx->idxHndls, idx);
//    newDbHndl->hndlBits = idxDbHndl->hndlBits;

//	if (idx >= docStore->idxMax)
//		docStore->idxMax = idx + 1;

//	abandonValue(name);
    s.bits = vt_hndl;
    s.subType = (uint8_t)idxHndl->hndlType;
    s.hndl->hndlId.bits = idxDbHndl->hndlId.bits;
    releaseHandle(idxHndl);
    return s;
}

//  createCursor(index, options)


value_t js_createCursor(uint32_t args, environment_t *env) {
	value_t index, opts;
	Params params[MaxParam + 1];
    value_t cursor;
	DbCursor *dbCursor;
    Handle *cursHndl;
	Handle *idxHndl;
    DbMap *idxMap;
	value_t s;

	s.bits = vt_status;

	index = eval_arg (&args, env);

	if (!(idxHndl = js_handle(index, Hndl_anyIdx)))
        return s.status = DB_ERROR_handleclosed, s;
    else
        idxMap = MapAddr(idxHndl);

	// process options array

	opts = eval_arg (&args, env);
	processOptions(params, opts);
	abandonValue(opts);

	cursor.bits = vt_hndl;
    cursor.subType = Hndl_cursor;

	if ((s.status = (int)createCursor(cursor.hndl, idxHndl->hndl, params)))
		return s;

	if ((cursHndl = js_handle(cursor, Hndl_cursor)))
		dbCursor = ClntAddr(cursHndl);
	else {
		fprintf(stderr, "Error: createCursor => unable to bind cursor:Handle\n");
		return s.status = ERROR_script_internal, s;
	}

	releaseHandle(idxHndl);
	releaseHandle(cursHndl);
	return cursor;
}

//	openDocStore(database, options)

value_t js_openDocStore(uint32_t args, environment_t *env) {
	value_t database, opts, name, hndl;
	Params params[MaxParam + 1];
	DocStore *docStore;
	string_t *namestr;
	Handle *dbHndl, *docHndl;
    DbMap *dbMap;
	value_t s;

	s.bits = vt_status;

	database = eval_arg (&args, env);
    
	if (!(dbHndl = js_handle(database, Hndl_database))) {
       fprintf(stderr, "Error: openDocStore=> expecting database:handle => %s\n", strtype(database));
       return s.status = DB_ERROR_badhandle, s;
    } else
		dbMap = MapAddr(dbHndl);

    name = eval_arg(&args, env);

	if (name.type == vt_string)
		namestr = js_dbaddr(name, NULL);
	else {
		fprintf(stderr, "Error: openDocStore => expecting name:string => %s\n", strtype(name));
		return s.status = ERROR_script_internal, s;
	}

	// process options array

	opts = eval_arg (&args, env);
	processOptions(params, opts);
	abandonValue(opts);

	params[ArenaXtra].intVal = sizeof(DocStore);

	if ((s.status = (int)openDocStore(hndl.hndl, dbHndl->hndl, (char *)namestr->val, namestr->len, params)))
		return s;

	if (!(docHndl = bindHandle(hndl.hndl, Hndl_docStore)))
		return s.status = DB_ERROR_arenadropped, s;
    
	assignDocStoreIdx(docHndl);
	dbMap = MapAddr(docHndl);
	docStore = (DocStore *)(dbMap->arena + 1);
	docStore->blkSize = params[MvccBlkSize].wordVal;
	docStore->docType = params[RecordType].charVal;

	/*
	//	open indexes

	lockLatch(map->arenaDef->nameTree->latch);

	if ((entry = rbStart (map, pathStk, map->arenaDef->nameTree)))
	  do {
		DbHandle *idxHndl;
		Handle *handle;
		uint16_t idx;
		DbMap *childMap;

		if ((childMap = arenaRbMap(map, entry))) {
    	  if ((handle = makeHandle(childMap, 0, *map->arena->type, params[HndlXtra].intVal))) {
			idx = arrayAlloc(docHndl->map, docStore->idxHndls, sizeof(DbHandle));
			idxHndl = arrayEntry(docHndl->map, docStore->idxHndls, idx);
        	idxHndl->hndlBits = handle->hndlId.bits;

			if (idx >= docStore->idxMax)
				docStore->idxMax = idx + 1;
		  }
		}
	  } while ((entry = rbNext(docHndl->map, pathStk)));

	unlockLatch(map->arenaDef->nameTree->latch);
*/
	hndl.bits = vt_hndl;
	hndl.subType = Hndl_docStore;

	releaseHandle(dbHndl);
    releaseHandle(docHndl);
    abandonValue(name);
	return hndl;
}

value_t fcnDbDrop(value_t *args, value_t thisVal, environment_t *env) {
	bool dropDefs = false;
	DbHandle *hndl;
	value_t s;

	s.bits = vt_status;

	if( (hndl = (DbHandle *)baseObject(thisVal)->hndl) )
	  if (vec_cnt(args) && args->type == vt_bool)
		dropDefs = args->boolean;
		
	s.status = (int)dropArena (hndl, dropDefs);
	return s;
}

PropFcn builtinDbFcns[] = {
	{ fcnDbDrop, "drop" },
	{ NULL, NULL}
};

PropVal builtinDbProp[] = {
//	{ propDbOnDisk, "onDisk" },
	{ NULL, NULL}
};

PropFcn builtinCatalogFcns[] = {
	{ NULL, NULL}
};

PropVal builtinCatalogProp[] = {
	{ NULL, NULL}
};
