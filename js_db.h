#pragma once

#include "database/Hi-Performance-Timestamps/timestamps.h"

#include "database/db.h"
#include "database/db_api.h"
#include "database/db_arena.h"
#include "database/db_map.h"
#include "database/db_malloc.h"
#include "database/db_object.h"
#include "database/db_handle.h"
#include "database/db_cursor.h"
#include "database/db_iterator.h"
#include "database/mvcc_dbapi.h"
#include "database/mvcc_dbdoc.h"

//	user Document version retrieved from a docStore 

typedef struct {
  value_t value[1];  // cloned document root value
  uint32_t maxOffset;
} JsDoc;

JsDoc *jsDocAddr(value_t val);
Doc *mvccDoc(struct Document *rawDoc);
Ver *mvccVer(value_t val);

JsStatus writeRawDoc(Handle *docHndl, value_t val, ObjId *docId);
JsStatus writeMVCCDoc(Handle *docHndl, value_t val, ObjId *docId);
JsStatus writeDoc(Handle *docHndl, value_t val, ObjId *docId);
JsStatus badHandle(value_t hndl);

void marshalDoc(value_t doc, uint8_t *base, uint32_t offset, uint32_t docSize, value_t *val, bool fullClone);
uint32_t calcSize (value_t doc, bool fullClone);
Catalog *initHndlMap(char *path, int pathLen, char *name, bool onDisk, int numDocStores);
Handle *js_handle(value_t hndl, int hndlType);
Txn* js_fetchTxn(value_t hndl);