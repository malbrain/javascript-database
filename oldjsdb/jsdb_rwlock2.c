//	Simple Reader/Writer lock

#ifdef linux
#define _GNU_SOURCE
#include <unistd.h>
#include <limits.h>
#endif

#ifndef _WIN32
#include <sched.h>
#endif

#include "jsdb.h"
#include "jsdb_db.h"

#ifdef linux
#include <linux/futex.h>
#include <sys/syscall.h>

int sys_futex(volatile uint32_t *addr1, int op, int val1, struct timespec *timeout, void *addr2, int val3)
{
	return syscall(SYS_futex, addr1, op, val1, timeout, addr2, val3);
}

#define pause() asm volatile("pause\n": : : "memory")
#endif

void WriteLock(RWLock *lock, SpinType spin)
{
uint32_t backoff = 16;
RWLock prev;
int idx;

  while (1) {
#ifndef _WIN32
	prev.bits = __sync_fetch_and_or (lock->mutex, WRITER);
#else
	prev.bits = _InterlockedOr (lock->mutex, WRITER);
#endif
	while (prev.writer) {
	  // wait for readers to reach zero

	  if (!prev.readers) 
	  	return;

	  if (spin == SpinLock) {
		for (idx = 0; idx < backoff; idx++) {
		  if (lock->readers)
			return;
#ifdef _WIN32
		  YieldProcessor();
#else
		  pause();
#endif
		}

		if (backoff < 16*1024*1024)
		  backoff *= 2;

		continue;
	  }
	  // wait for zero reader count
#ifdef linux
	  prev.waitwrt2 = 1;
	  __sync_fetch_and_or (lock->mutex, WAITWRT2);
	  sys_futex (lock->mutex, FUTEX_WAIT_BITSET, prev.bits, NULL, NULL, WaitWrt2);
#elif defined(_WIN32)
	  SwitchToThread();
#else
	  sched_yield();
#endif
	}

  // wait for writer mutex
#ifdef linux
  prev.waitwrt1 = 1;
  __sync_fetch_and_or (lock->mutex, WAITWRT1);
  sys_futex (lock->mutex, FUTEX_WAIT_BITSET, prev.bits, NULL, NULL, WaitWrt1);
#elif defined(_WIN32)
  SwitchToThread();
#else
  sched_yield();
#endif
  }
}

void rwUnlock(RWLock *lock, RWType type)
{
RWLock prev;

  if (type == RWWrite) {
	// release write lock
#ifndef _WIN32
	prev.bits = __sync_fetch_and_and (lock->mutex, ~WRITER);
#else
	prev.bits = _InterlockedAnd (lock->mutex, ~WRITER);
#endif

#ifdef linux
	if (prev.bits & WAITRD) {
	  __sync_fetch_and_and (lock->mutex, ~WAITRD);
	  sys_futex (lock->mutex, FUTEX_WAKE_BITSET, INT_MAX, NULL, NULL, WaitRd);
	} else if (prev.bits & WAITWRT1) {
	  __sync_fetch_and_and (lock->mutex, ~WAITWRT1);
	  sys_futex (lock->mutex, FUTEX_WAKE_BITSET, 1, NULL, NULL, WaitWrt1);
	}
#endif
	return;
  } else {
	// release read lock

#ifndef _WIN32
	prev.bits = __sync_fetch_and_add (lock->mutex, -READER) - READER;
#else
	prev.bits = _InterlockedAdd (lock->mutex, -READER);
#endif
	if (prev.readers)
		return;
#ifdef linux
  if (prev.bits & WAITWRT2) {
	__sync_fetch_and_and (lock->mutex, ~WAITWRT2);
	sys_futex (lock->mutex, FUTEX_WAKE_BITSET, 1, NULL, NULL, WaitWrt2);
  }
#endif
  }
}

void readLock (RWLock *lock, SpinType spin)
{
uint32_t backoff;
RWLock prev;
int idx;

 while ((backoff = 16)) {
  // wait until writer unlocks

  while ((lock->writer)) {
	if (spin == SpinLock) {
	  for (idx = 0; idx < backoff; idx++)
#ifdef _WIN32
		YieldProcessor();
#else
		pause();
#endif
	  if (backoff < 16*1024*1024)
		backoff *= 2;
    } else {
	  for (idx = 0; idx < backoff; idx++)
#ifdef _WIN32
		YieldProcessor();
#else
		pause();
#endif
	  if (!backoff)
		continue;

	  backoff = 0;
#ifdef linux
	  prev.bits = __sync_fetch_and_or (lock->mutex, WAITRD);
	  prev.waitrd = 1;
	  sys_futex (lock->mutex, FUTEX_WAIT_BITSET, prev.bits, NULL, NULL, WaitRd);
#elif defined(_WIN32)
	  SwitchToThread();
#else
	  sched_yield();
#endif
	}
   }
  // if writers gone,
  // take reader lock

#ifndef _WIN32
  __sync_fetch_and_add (lock->mutex, READER);
#else
  _InterlockedAdd (lock->mutex, READER);
#endif
  if (!lock->writer)
	return;

  // otherwise, try again from the top

#ifndef _WIN32
  __sync_fetch_and_add (lock->mutex, -READER);
#else
  _InterlockedAdd (lock->mutex, -READER);
#endif
 }
}
