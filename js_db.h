#pragma once

#include "database/db.h"
#include "database/db_api.h"
#include "database/db_arena.h"
#include "database/db_map.h"
#include "database/db_malloc.h"
#include "database/db_object.h"
#include "database/db_handle.h"
#include "database/db_cursor.h"
#include "database/db_iterator.h"

//  Pending TXN action

typedef enum {
	TxnNone = 0,		// not in a txn
	TxnInsert,			// insert new doc
	TxnDelete,			// delete the doc
	TxnUpdate,			// update the doc
	TxnFinished			// txn committed/rollback
} TxnAction;

typedef enum {
	TxnDone,			// fully committed
	TxnGrow,			// reading and upserting
	TxnShrink			// committing
} TxnState;

//	Database transactions: housed in database ObjId slots

typedef struct {
	uint64_t refCnt[1];	// number TxnId references
	DbAddr frame[1];	// insert/update DocId in Txn
	uint64_t nextTxn;	// nested txn next
	uint64_t timestamp;	// transaction timestamp
	TxnState state[1];	// state of txn/latch bit
} Txn;

// transaction errors

#define TXN_ERR_rw_conflict ((Ver *)(1ULL))

// javascript version header
//	occurs immediately after Ver members

typedef struct Ver_ {
  struct {				// end of record marker
	uint32_t verSize;   // version size
	uint32_t offset;	// offset from beginning
  };

  value_t rec[1];		// base document (object)
  DbAddr keys[1];		// document key addresses
  uint64_t verNo;		// version number
  uint64_t maxRdTs;		// maximum reader timestamp
  uint64_t commitTs;	// version timestamp set on commit
  uint8_t newerVer;		// newer committed version exists
  uint8_t deferred;		// some keys w/deferred constraints
} Ver;

typedef struct {
	ObjId docId;		// document ID
	ObjId txnId;		// insert/version txn ID
	uint64_t verNo;		// current update version number
	uint64_t writeTs;	// uncommitted update timestamp
	DbAddr docAddr;	 	// doc docStore arena address
	DbAddr prevAddr;	// previous doc version set
	uint32_t lastVer;   // offset of most recent version
	TxnAction pending;  // pending document action
} Doc;

// database docStore handle extension

typedef struct {
	DbAddr idxHndls[1];	// skip list of index handles by id
} DocStore;

//	committed == not reader
//	reader == even
//	writer == odd

#define isCommitted(ts) (ts&1)

DbStatus addDocToTxn(ObjId txnId, ObjId docId);
int64_t getTimestamp(bool commit);
Txn *fetchTxn(ObjId txnId);
