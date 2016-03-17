#include "jsdb.h"
#include "jsdb_db.h"
#include "jsdb_btree.h"

#ifdef linux
int sys_futex(void *addr1, int op, int val1, struct timespec *timeout, void *addr2, int val3)
{
	return syscall(SYS_futex, addr1, op, val1, timeout, addr2, val3);
}
#endif

void bt_mutexlock(Mutex *latch)
{
uint32_t idx, waited = 0;
Mutex prev[1];

 while( 1 ) {
  for( idx = 0; idx < 100; idx++ ) {
#ifdef unix
	*prev->value = __sync_fetch_and_or (latch->value, 1);
#else
	*prev->value = InterlockedOr (latch->value, 1);
#endif
	if( !*prev->bits->xcl ) {
	  if( waited )
#ifdef unix
		__sync_fetch_and_sub (latch->bits->waiters, 1);
#else
		InterlockedDecrement16 (latch->bits->waiters);
#endif
	  return;
	}
  }

  if( !waited ) {
#ifdef unix
	__sync_fetch_and_add (latch->bits->waiters, 1);
#else
	_InterlockedIncrement16 (latch->bits->waiters);
#endif
	*prev->bits->waiters += 1;
	waited++;
  }

#ifdef linux
  sys_futex (latch->value, FUTEX_WAIT, *prev->value, NULL, NULL, 0);
#elif defined(_WIN32)
  SwitchToThread();
#else
  sched_yield();
#endif
 }
}

int bt_mutextry(Mutex *latch)
{
#ifdef unix
	return !__sync_lock_test_and_set (latch->bits->xcl, 1);
#else
	return !InterlockedExchange16 (latch->bits->xcl, 1);
#endif
}

void bt_releasemutex(Mutex *latch)
{
Mutex prev[1];

#ifdef unix
	*prev->value = __sync_fetch_and_and (latch->value, 0xffff0000);
#else
	*prev->value = InterlockedAnd (latch->value, 0xffff0000);
#endif

	if( *prev->bits->waiters )
#ifdef linux
		sys_futex( latch->value, FUTEX_WAKE, 1, NULL, NULL, 0 );
#elif defined(_WIN32)
  		SwitchToThread();
#else
  		sched_yield();
#endif
}

//	reader/writer lock implementation

void WriteLock (RWLock *lock)
{
	bt_mutexlock (lock->xcl);
	bt_mutexlock (lock->wrt);
	bt_releasemutex (lock->xcl);
}

void WriteRelLock (RWLock *lock)
{
	bt_releasemutex (lock->wrt);
}

void ReadLock (RWLock *lock)
{
	bt_mutexlock (lock->xcl);

#ifdef unix
	if( !__sync_fetch_and_add (&lock->readers, 1) )
#else
	if( !(InterlockedIncrement16 (&lock->readers)-1) )
#endif
		bt_mutexlock (lock->wrt);

	bt_releasemutex (lock->xcl);
}

void ReadRelLock (RWLock *lock)
{
#ifdef unix
	if( __sync_fetch_and_sub (&lock->readers, 1) == 1 )
#else
	if( !InterlockedDecrement16 (&lock->readers) )
#endif
		bt_releasemutex (lock->wrt);
}

