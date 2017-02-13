#pragma once

#include "database/db.h"
#include "database/db_api.h"
#include "database/db_arena.h"
#include "database/db_map.h"
#include "database/db_malloc.h"
#include "database/db_object.h"
#include "database/db_handle.h"
#include "database/db_index.h"
#include "database/db_iterator.h"

typedef enum {
    DocUnused = 0,
    DocActive,
    DocInsert,
    DocDelete,
    DocDeleted
} DocState;

// javascript version header
//	occurs immediately after Ver members

typedef struct Ver_ {
  struct VerEnd {		// end of record marker
    uint32_t verSize;   // version size
    uint32_t offset;    // offset from beginning
    uint64_t version;   // document version
  };

  ObjId txnId;        	// insert/version txn ID
  value_t rec[1];		// base document (object)
  value_t keys[1];		// document keys (object)
  uint64_t timestamp;	// version timestamp
} Ver;

typedef struct {
    uint32_t refCnt[1]; // active references to the document
    DbAddr prevAddr;    // previous doc version set
    DbAddr docAddr;     // doc version arena address
    ObjId delId;        // delete txn ID
    ObjId docId;        // document ID
    uint32_t lastVer;   // offset of most recent version
    DocState state;     // document state
} Doc;

//	Database transactions: housed in database ObjId slots

typedef struct {
	DbAddr frame[1];	// contains versions in the TXN
	uint64_t beginTs;	// txn beginning timestamp
	uint64_t commitTs;	// txn committed timestamp
} Txn;

//  txn command enum:

typedef enum {
	TxnAddDoc,	// add a new document
	TxnDelDoc,	// delete the document
	TxnUpdDoc	// add new version to document
} TxnCmd;
	
void addVerToTxn(DbMap *database, Txn *txn, Ver *ver, TxnCmd cmd);
