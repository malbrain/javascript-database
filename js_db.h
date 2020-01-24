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

//	Document retrieved from a docStore

struct Document {
  uint32_t refCnt[1];
  ObjId docId;
  DbAddr ourAddr;
  value_t value[1];		// clone document value
  uint32_t docLen;
  uint8_t base[];
};

JsStatus appendDoc(Handle *docHndl, value_t val, ObjId *docId);
value_t makeDocument(ObjId docId, DbMap *docMap);

void marshalDoc(value_t doc, uint8_t *base, uint32_t offset, uint32_t docSize, value_t *val, bool fullClone, uint8_t *src);
uint32_t calcSize (value_t doc, bool fullClone, uint8_t *src);
