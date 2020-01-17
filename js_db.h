#pragma once

#include "database/Hi-Performance-Timestamps/timestamps.h"
//#include "timestamps.h"

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
#include "database/mvcc_dbidx.h"

//	Document version retrieved/updated from a docStore

struct Document {
	DbHandle hndl[1];		// docStore handle
	value_t value;		// version value
	uint8_t *base;			// pointer to doc base
	Ver *ver;				// pointer to version
};
	
value_t makeDocument(Ver *ver, DbHandle hndl[1]);

void marshalDoc(value_t doc, uint8_t *base, uint32_t offset, uint32_t docSize, value_t *val, bool fullClone, uint8_t *src);
uint32_t calcSize (value_t doc, bool fullClone, uint8_t *src);
