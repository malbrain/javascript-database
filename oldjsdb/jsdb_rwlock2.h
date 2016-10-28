#pragma once

#include "jsdb_util.h"

//	definition for reader/writer lock implementation
//  n.b. this is write prefering RW lock

#define READER 256
#define WRITER 1
#define WAITRD 2
#define WAITWRT1 4
#define WAITWRT2 8

typedef struct {
  volatile union {
	struct {
	  uint32_t writer:1;
	  uint32_t waitrd:1;		// reader waiting for writer bit
	  uint32_t waitwrt1:1;		// writer waiting for writer bit
	  uint32_t waitwrt2:1;		// writer waiting for reader cnt
	  uint32_t filler:4;
	  uint32_t readers:24;
	};
	int32_t mutex[1];
	uint32_t bits;
  };
} RWLock;

#ifdef linux
#define WaitWrt1 1
#define WaitWrt2 2
#define WaitRd 4
#endif

typedef enum {
	RWRead,
	RWWrite
} RWType;

typedef enum {
	SpinLock,
	YieldLock
} SpinType;

void writeLock (RWLock *lock, SpinType yield);
void readLock (RWLock *lock, SpinType yield);
void rwUnlock (RWLock *lock, RWType type);
