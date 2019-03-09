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

//	Timestamp object
//	must be 16 byte aligned

typedef 
#ifdef _WIN32
__declspec(align(16))
#endif
struct {
  union {
	int64_t lowBits;
	struct {
		uint32_t sequence;		// current sequence in epoch
		uint16_t processId;		// process/thread ID on machine
		uint16_t clusterId;		// machine ID in cluster
	};
  };
  int64_t epoch;
}
#ifndef _WIN32
__attribute__((aligned(16),packed))
#endif
Timestamp;

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
	TxnShrink,			// committing
	TxnRollback			// roll back
} TxnState;

typedef enum {
	TxnKill = 0,		// txn step removed
	TxnHndl,			// txn step is a docStore handle
	TxnDoc				// txn step is a docId
} TxnType;

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
	ObjId hndlId[1];		// current DocStore handle
	DbAddr rdrFrame[1];		// head read set DocIds
	DbAddr rdrFirst[1];		// first read set DocIds
	DbAddr docFrame[1];		// head write set DocIds
	DbAddr docFirst[1];		// first write set DocIds
	Timestamp reader[1];	// txn begin timestamp
	Timestamp commit[1];	// txn commit timestamp
	Timestamp pstamp[1];	// predecessor high water
	Timestamp sstamp[1];	// successor low water
	uint64_t nextTxn;		// nested txn next
	uint32_t wrtCount;		// size of write set
	union {
	  struct {
		char isolation;
		volatile char state[1];
	  };
	  TxnCC disp:8;			// display isolation mode
	};
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
  Timestamp commit[1];	// commit timestamp
  Timestamp pstamp[1];	// highest access timestamp
  Timestamp sstamp[1];	// successor's commit timestamp, or infinity
  uint8_t deferred;		// some keys w/deferred constraints
} Ver;

typedef struct {
	ObjId docId;			// document ID
	ObjId txnId;			// pending uncommitted txn ID
	DbAddr ourAddr;			// address of this version set
	DbAddr prevAddr;		// previous doc-version set
	DbAddr nextAddr;		// next doc-version set
	TxnAction op;			// pending document action/committing bit
	uint32_t lastVer;		// offset of most recent version
	uint32_t refCnt[1];		// number of latching doc references
	uint16_t xtraAddr;		// our docStore Id
} Doc;

//  cursor/iterator handle extension

typedef struct {
	ObjId txnId;
	DbAddr deDup[1];		// de-duplication set membership
	DbHandle hndl[1];		// docStore DbHandle
	Timestamp reader[1];	// read timestamp
	TxnCC isolation;		// txn isolation mode
} JsMvcc;

//	Document version retrieved/updated from a docStore

struct Document {
	DbHandle hndl[1];		// docStore handle
	value_t value;		// version value
	uint8_t *base;			// pointer to doc base
	Ver *ver;				// pointer to version
};
	
//	catalog concurrency parameters

typedef struct {
	TxnCC isolation;
} CcMethod;

// database docStore handle extension

typedef struct {
	DbAddr idxHndls[1];	// array of index handles
	uint16_t idxMax;
} DocStore;

//	timestamps

//	committed == not reader
//	reader == even
//	writer == odd

#define compareTs(ts1, ts2) (ts1->epoch - ts2->epoch ? ts1->epoch - ts2->epoch : ts1->lowBits - ts2->lowBits)

Txn *fetchTxn(ObjId txnId);
void newTs(Timestamp *ts, Timestamp *gen, bool reader);
void processOptions(Params *params, value_t options);
value_t makeDocument(Ver *ver, DbHandle hndl[1]);

uint64_t beginTxn(Params *params, uint64_t *txnBits, Timestamp *tsGen);
JsStatus rollbackTxn(Params *params, uint64_t *txnBits);
JsStatus commitTxn(Params *params, uint64_t *txnBits, Timestamp *tsGen);

JsStatus addDocWrToTxn(ObjId txnId, ObjId docId, Ver *ver, Ver *prevVer, uint64_t hndlBits);
JsStatus findDocVer(DbMap *docStore, Doc *doc, JsMvcc *jsMvcc);
JsStatus updateDoc(Handle **idxHndls, document_t *document, ObjId txnId, Timestamp *tsGen);
JsStatus insertDoc(Handle **idxHndls, value_t val, DbAddr *docSlot, uint64_t docBits, ObjId txnId, Ver *prevVer, Timestamp *tsGen, uint8_t *src);

void marshalDoc(value_t doc, uint8_t *base, uint32_t offset, uint32_t docSize, value_t *val, bool fullClone, uint8_t *src);
uint32_t calcSize (value_t doc, bool fullClone, uint8_t *src);
