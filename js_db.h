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
	Done = 0,			// not in a txn
	Insert,				// insert new doc
	Delete,				// delete the doc
	Update,				// update the doc
	Committing = 128	// version being committed
} TxnAction;

typedef enum {
	TxnCommit,			// fully committed
	TxnGrow,			// reading and upserting
	TxnShrink			// committing
} TxnState;

typedef enum {
	NotSpecified,
	SnapShot,
	ReadCommitted,
	Serializable
} TxnCC;

typedef enum {
	Concurrency = UserParams + 1,
} UserParmSlots;

//	Database transactions: housed in database ObjId slots

typedef struct {
	DbAddr rdrFrame[1];	// read DocIds
	DbAddr docFrame[1];	// insert/update DocIds
	uint64_t timestamp;	// read timestamp
	uint64_t nextTxn;	// nested txn next
	uint32_t wrtCount;	// size of write set
	TxnCC isolation:8;	// txn isolation mode
	volatile char state[1];
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
	ObjId docId;			// document ID
	ObjId txnId;			// pending uncommitted txn ID
	DbAddr ourAddr;			// address of this version set
	DbAddr prevAddr;		// previous doc-version set
	DbAddr newerAddr;		// newer doc-version set
	uint32_t refCnt[1];		// number of latching doc references
	uint32_t lastVer;		// offset of most recent version
	TxnAction op:8;			// pending document action/committing bit
} Doc;

//  cursor/iterator extension

typedef struct {
	uint64_t ts;
	ObjId txnId;
	DbAddr deDup[1];	// de-duplication set membership
	DbHandle hndl[1];	// docStore DbHandle
	TxnCC isolation:8;	// txn isolation mode
} JsMvcc;

//	Document version retrieved/updated from a docStore

typedef struct {
	value_t value[1];		// version value
	struct Ver_ *ver;		// pointer to version
	uint64_t addrBits;		// version set addr
	DbHandle hndl[1];		// docStore DbHandle
} document_t;
	
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

uint64_t getSnapshotTimestamp(JsMvcc *jsMvcc, bool commit);
value_t makeDocument(Ver *ver, DbHandle hndl[1]);
