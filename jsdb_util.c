#ifdef linux
#define _GNU_SOURCE
#endif

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <errno.h>
#include <sched.h>
#endif

#include "jsdb.h"
#include "jsdb_db.h"
#include "jsdb_util.h"

//  assemble filename path

int getPath(char *path, int off, value_t name, DbMap *parent, uint32_t segNo) {
	int len, idx;

	path[--off] = 0;
#ifdef _WIN32
	idx = off;
	off -= 8;

	memcpy (path + off, ".seg0000", 8);

	while (segNo) {
		path[--idx] += segNo & 0xf;
		segNo /= 10;
	}
#endif
	if (off > name.aux)
		off -= name.aux;
	else
		return -1;

	memcpy(path + off, name.str, name.aux);

	while (parent) {
		if (parent->name.aux)
			path[--off] = '.';
		else
			break;

		if( off > parent->name.aux)
			off -= parent->name.aux;
		else
			return -1;

		memcpy(path + off, parent->name.str, parent->name.aux);
		parent = parent->parent;
	}

	len = strlen("data/");

	if (off > len)
		off -= len;
	else
		return -1;

	memcpy(path + off, "data/", len);
	return off;
}

void waitNonZero(volatile char *zero) {
	while (!*zero)
#ifndef _WIN32
			relax();
#else
			SwitchToThread();
#endif
}

void lockLatch(volatile char* latch) {
#ifndef _WIN32
	while (__sync_fetch_and_or(latch, MUTEX_BIT) & MUTEX_BIT) {
#else
	while (_InterlockedOr8(latch, MUTEX_BIT) & MUTEX_BIT) {
#endif
		do
#ifndef _WIN32
			relax();
#else
			SwitchToThread();
#endif
		while (*latch & MUTEX_BIT);
	}
}

void unlockLatch(volatile char* latch) {
#ifndef _WIN32
	__sync_fetch_and_and(latch, ~MUTEX_BIT);
#else
	InterlockedAnd8( latch, ~MUTEX_BIT);
#endif
}

uint64_t atomicAdd64(volatile uint64_t *value, uint64_t amt) {
#ifndef _WIN32
	return __sync_fetch_and_add(value, amt) + amt;
#else
	return InterlockedAdd64( value, amt);
#endif
}

uint32_t atomicAdd32(volatile uint32_t *value, uint32_t amt) {
#ifndef _WIN32
	return __sync_fetch_and_add(value, amt) + amt;
#else
	return InterlockedAdd( value, amt);
#endif
}

uint64_t atomicOr64(volatile uint64_t *value, uint64_t amt) {
#ifndef _WIN32
	return __sync_fetch_and_or (value, amt);
#else
	return InterlockedOr64( value, amt);
#endif
}

uint32_t atomicOr32(volatile uint32_t *value, uint32_t amt) {
#ifndef _WIN32
	return __sync_fetch_and_or(value, amt);
#else
	return InterlockedOr( value, amt);
#endif
}

#ifdef _WIN32
void lockArena (HANDLE hndl, char *path) {
OVERLAPPED ovl[1];

	memset (ovl, 0, sizeof(ovl));
	ovl->OffsetHigh = 0x80000000;

	if (LockFileEx (hndl, LOCKFILE_EXCLUSIVE_LOCK, 0, sizeof(DbArena), 0, ovl))
		return;

	fprintf (stderr, "Unable to lock %s, error = %d", path, GetLastError());
	exit(1);
}
#else
void lockArena (int hndl, char *path) {
struct flock lock[1];

	memset (lock, 0, sizeof(lock));
	lock->l_len = sizeof(DbArena);
	lock->l_type = F_WRLCK;

	if (!fcntl(hndl, F_SETLKW, lock))
		return;

	fprintf (stderr, "Unable to lock %s, error = %d", path, errno);
	exit(1);
}
#endif

#ifdef _WIN32
void unlockArena (HANDLE hndl, char *path) {
OVERLAPPED ovl[1];

	memset (ovl, 0, sizeof(ovl));
	ovl->OffsetHigh = 0x80000000;

	if (UnlockFileEx (hndl, 0, sizeof(DbArena), 0, ovl))
		return;

	fprintf (stderr, "Unable to unlock %s, error = %d", path, GetLastError());
	exit(1);
}
#else
void unlockArena (int hndl, char *path) {
struct flock lock[1];

	memset (lock, 0, sizeof(lock));
	lock->l_len = sizeof(DbArena);
	lock->l_type = F_UNLCK;

	if (!fcntl(hndl, F_SETLKW, lock))
		return;

	fprintf (stderr, "Unable to unlock %s, error = %d", path, errno);
	exit(1);
}
#endif

void *mapMemory (DbMap *map, uint64_t offset, uint64_t size, uint32_t segNo) {
	void *mem;

#ifndef _WIN32
	int flags = MAP_SHARED;

	if( map->hndl[0] < 0 )
		flags |= MAP_ANON;

	mem = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_ANON | MAP_SHARED, map->hndl[0], offset);

	if (mem == MAP_FAILED) {
		fprintf (stderr, "Unable to mmap %s, offset = %ld, error = %d", map->name.str, offset, errno);
		return NULL;
	}
#else
	if (!map->onDisk)
		return VirtualAlloc(NULL, size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	char pathBuff[MAX_path], *path;
	int pathOff, pathSeg = segNo;
	int idx = sizeof(pathBuff);

	pathOff = getPath(pathBuff, idx, map->name, map->parent, segNo);
	path = pathBuff + pathOff;

	map->hndl[segNo] = CreateFile(path, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	
	if (map->hndl[segNo] == INVALID_HANDLE_VALUE) {
		fprintf(stderr, "Unable to create/open %s, error = %d\n", path, GetLastError());
		return NULL;
	}

	if (!(map->maphndl[segNo] = CreateFileMapping(map->hndl[segNo], NULL, PAGE_READWRITE, (DWORD)(size >> 32), (DWORD)(size), NULL))) {
		fprintf (stderr, "Unable to CreateFileMapping %s, size = %ld, error = %d\n", map->name.str, size, GetLastError());
		return NULL;
	}

	mem = MapViewOfFile(map->maphndl[segNo], FILE_MAP_WRITE, 0, 0, size);

	if (!mem) {
		fprintf (stderr, "Unable to CreateFileMapping %s, size = %d, error = %d\n", map->name.str, size, GetLastError());
		return NULL;
	}
#endif

	return mem;
}

void unmapSeg (DbMap *map, uint32_t segNo) {
#ifndef _WIN32
	munmap(map->base[segNo], map->arena->segs[segNo].size);
#else
	if (!map->onDisk) {
		VirtualFree(map->base[segNo], 0, MEM_RELEASE);
		return;
	}

	UnmapViewOfFile(map->base[segNo]);
	CloseHandle(map->maphndl[segNo]);
#endif
}

uint64_t CompareAndSwap(uint64_t* target, uint64_t compare_val, uint64_t swap_val) {
#ifndef _WIN32
	return __sync_val_compare_and_swap(target, compare_val, swap_val);
#else
	return InterlockedCompareExchange64((volatile __int64*)target, swap_val, compare_val);
#endif
}

#ifdef _WIN32
uint32_t getCpuCount() {
	SYSTEM_INFO info[1];

	GetSystemInfo (info);
	return info->dwNumberOfProcessors;
}
#endif

void kill_slot(volatile char *latch) {
#ifndef _WIN32
	__sync_fetch_and_or(latch, DEAD_BIT);
#else
	InterlockedOr8(latch, DEAD_BIT);
#endif
}

void art_yield() {
#ifndef _WIN32
	sched_yield();
#else
	SwitchToThread();
#endif
}

#ifdef __APPLE__
#include <sys/types.h>
#include <sys/sysctl.h>
#include <pthread.h>

uint32_t getCpuCount() {
	int count;
	size_t count_len = sizeof(count);
	sysctlbyname("hw.logicalcpu_max", &count, &count_len, NULL, 0);
	return count;
}
#endif

#ifdef linux
#include <sys/sysinfo.h>

uint32_t getCpuCount() {
	return get_nprocs();
}
#endif

uint32_t getSet(DbMap *map) {
#ifdef __APPLE__
	uint64_t tid;
	pthread_threadid_np(NULL, &tid);
	return (uint32_t)(tid % map->cpuCount);
#else
#ifndef _WIN32
	return sched_getcpu() % map->cpuCount;
#else
	return GetCurrentProcessorNumber() % map->cpuCount;
#endif
#endif
}

bool fileExists(char *path) {
#ifdef _WIN32
	int attr = GetFileAttributes(path);

	if( attr == 0xffffffff)
		return false;

	if (attr & FILE_ATTRIBUTE_DIRECTORY)
		return false;

	return true;
#else
	return !access(path, F_OK);
#endif
}
