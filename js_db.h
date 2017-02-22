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

//  Pending TXN action

typedef enum {
	TxnIdle,			// txn committed/rollback
	TxnInsert,			// insert new doc
	TxnDelete,			// delete the doc
	TxnUpdate			// update the doc
} TxnAction;

typedef enum {
	TxnDone,
	TxnGrow,
	TxnShrink,
} TxnState;

//	Database transactions: housed in database ObjId slots

typedef struct {
	DbAddr frame[1];	// list of DocId in the TXN
	uint64_t readTs;	// txn read timestamp
	uint64_t commitTs;	// txn commit timestamp
	TxnState state[1];	// state of txn/latch bit
} Txn;

// javascript version header
//	occurs immediately after Ver members

typedef struct Ver_ {
  struct {				// end of record marker
	uint32_t verSize;   // version size
	uint32_t offset;	// offset from beginning
	uint64_t verNo;		// version number
  };

  ObjId txnId;			// insert/version txn ID
  value_t rec[1];		// base document (object)
  DbAddr keys[1];		// document key addresses
  uint64_t timestamp;	// version timestamp
} Ver;

typedef struct {
	ObjId docId;		// document ID
	uint64_t verNo;		// current update version number
	DbAddr docAddr;	 	// doc docStore arena address
	DbAddr prevAddr;	// previous doc version set
	uint32_t lastVer;   // offset of most recent version
	TxnAction pending;  // pending document action
} Doc;

// database docStore handle extension

typedef struct {
	DbAddr idxHndls[1];	// skip list of index handles by id
} DocStore;

DbStatus addDocToTxn(DbMap *database, Txn *txn, ObjId docId);
