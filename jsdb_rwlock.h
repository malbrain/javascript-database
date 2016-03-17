#pragma once

#include "jsdb_util.h"

typedef struct {
  union {
	struct {
	  volatile uint16_t xcl[1];
	  volatile uint16_t waiters[1];
	} bits[1];
	uint32_t value[1];
  };
} Mutex;

//	definition for reader/writer lock implementation

typedef struct {
  uint16_t readers;
  Mutex xcl[1];
  Mutex wrt[1];
} RWLock;


void bt_mutexlock(Mutex *latch);
int bt_mutextry(Mutex *latch);
void bt_releasemutex(Mutex *latch);

void WriteLock (RWLock *lock);
void WriteRelLock (RWLock *lock);
void ReadLock (RWLock *lock);
void ReadRelLock (RWLock *lock);
