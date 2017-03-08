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
	TxnDone = 0,		// not in a txn
	TxnInsert,			// insert new doc
	TxnDelete,			// delete the doc
	TxnUpdate			// update the doc
} TxnAction;

typedef enum {
	TxnCommit,			// fully committed
	TxnGrow,			// reading and upserting
	TxnShrink			// committing
} TxnState;

typedef enum {
	SnapShot = 0,		// the default
	Serializable
} TxnCC;

//	Database transactions: housed in database ObjId slots

typedef struct {
	uint64_t refCnt[1];	// number TxnId references
	DbAddr docFrame[1];	// insert/update DocIds
	DbAddr rdrFrame[1];	// read DocIds
	uint64_t wrtCount;	// size of write set
	uint64_t timestamp;	// transaction timestamp
	uint64_t nextTxn;	// nested txn next
	TxnState state[1];	// state of txn/latch bit
} Txn;

// javascript document version header

typedef struct Ver_ {
  struct {				// end of record marker
	uint32_t verSize;   // version size
	uint32_t offset;	// offset from beginning
  };

  value_t rec[1];		// base document (object)
  DbAddr keys[1];		// document key addresses
  uint64_t verNo;		// version number
  uint64_t readerTs;	// highest reader timestamp
  uint64_t commitTs;	// version timestamp (set on commit)
  uint8_t deferred;		// some keys w/deferred constraints
} Ver;

typedef struct {
	ObjId docId;		// document ID
	ObjId txnId;		// insert/version txn ID
	DbAddr prevAddr;	// previous doc-version set
	uint32_t lastVer;   // offset of most recent version
	TxnAction pending;  // pending document action
} Doc;

//	catalog concurrency parameters

typedef struct {
	TxnCC isolation;
} CcMethod;

// database docStore handle extension

typedef struct {
	DbAddr idxHndls[1];	// skip list of index handles by id
} DocStore;

//	SnapShot timestamps

//	committed == not reader
//	reader == even
//	writer == odd

int64_t getSnapshotTimestamp(ObjId txnId, bool commit);
