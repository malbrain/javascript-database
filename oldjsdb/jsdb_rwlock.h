#pragma once

#include "jsdb_util.h"

typedef union {
	struct {
	  volatile short xcl[1];
	  volatile short waiters[1];
	} bits[1];
#ifdef _WIN32
	volatile long value[1];
#else
	volatile uint32_t value[1];
#endif
} Mutex;

//	definition for reader/writer lock implementation

typedef struct {
  volatile short readers;
  uint8_t type;
  Mutex xcl[1];
  Mutex wrt[1];
} RWLock;


void bt_mutexlock(Mutex *latch);
int bt_mutextry(Mutex *latch);
void bt_releasemutex(Mutex *latch);

void writeLock (RWLock *lock);
void readLock (RWLock *lock);
void rwUnlock (RWLock *lock);
