#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <errno.h>
#endif

#include "jsdb.h"
#include "jsdb_arena.h"
#include "jsdb_util.h"

//
//  create/open a documentstore/index/engine arena on disk
//

bool mapSeg (DbMap *map, uint32_t currSeg);
void mapZero(DbMap *map, uint64_t segSize);
void mapAll (DbMap *map);

DbMap* openMap(uint8_t *name, uint32_t nameLen, uint32_t baseSize, uint64_t initSize, bool inMem) {
	DbMap *map  = jsdb_alloc(sizeof(DbMap) + nameLen, true);
	uint32_t segOffset = sizeof(DbArena) + baseSize;
	DbArena *segZero;
	int32_t amt = 0;

	initSize += 4095;
	initSize &= -4096;

	if (!initSize)
		initSize = SEGZERO_size;

	segOffset += 7;
	segOffset &= -8;

	map->cpuCount = getCpuCount();
	memcpy (map->fName, name, nameLen);
	*map->inMem = inMem;

	if (!inMem) {
#ifdef _WIN32
		map->hndl = CreateFile (map->fName, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (map->hndl == INVALID_HANDLE_VALUE) {
			fprintf (stderr, "Unable to open/create %s, error = %d", name, GetLastError());
			free(map);
			return NULL;
		}

		segZero = VirtualAlloc(NULL, sizeof(DbArena), MEM_COMMIT, PAGE_READWRITE);
		lockArena(map);
		if (!ReadFile(map->hndl, segZero, sizeof(DbArena), &amt, NULL)) {
			fprintf (stderr, "Unable to read %d bytes from %s, error = %d", sizeof(DbArena), map->fName, errno);
			VirtualFree(segZero, 0, MEM_RELEASE);
			free(map);
			return NULL;
		}
#else
		map->hndl = open (map->fName, O_RDWR | O_CREAT, 0666);

		if (map->hndl == -1) {
			fprintf (stderr, "Unable to open/create %s, error = %d", map->fName, errno);
			free(map);
			return NULL;
		}

		// read first part of segment zero if it exists

		segZero = valloc(sizeof(DbArena));
		lockArena(map);

		if ((amt = pread(map->hndl, segZero, sizeof(DbArena), 0)) {
			if (amt < sizeof(DbArena)) {
				fprintf (stderr, "Unable to read %d bytes from %s, error = %d", sizeof(DbArena), map->fName, errno);
				unlockArena(map);
				free(segZero);
				free(map);
				return NULL;
			}
		}
#endif
	} else
#ifdef _WIN32
		map->hndl = INVALID_HANDLE_VALUE;
#else
		map->hndl = -1;
#endif

	//  if segment zero exists, map the arena

	if (amt) {
		unlockArena(map);
		mapZero(map, segZero->segs->segSize);
#ifdef _WIN32
		VirtualFree(segZero, 0, MEM_RELEASE);
#else
		free(segZero);
#endif
		return map;
	}

#ifdef _WIN32
	VirtualFree(segZero, 0, MEM_RELEASE);
#else
	free(segZero);
#endif

	//  create initial segment on unix, windows will automatically do it

#ifndef _WIN32
	if (ftruncate(map->hndl, initSize)) {
		fprintf (stderr, "Unable to initialize file %s, error = %d", map->fName, errno);
		exit(1);
	}
#endif

	//  initialize arena segment zero

	mapZero(map, initSize);
	map->arena->nextObject.offset = segOffset;
	map->arena->segs->segSize = initSize;

	unlockArena(map);
	return map;
}

//  initialize arena from disk

void mapZero(DbMap *map, uint64_t segSize) {

	map->arena = mapMemory (map, 0, segSize, 0);
	map->base[0] = (char *)map->arena;

	mapAll(map);
}

//  extend arena into new segment
//  return FALSE if out of memory

bool newSeg(DbMap *map, uint32_t minSize) {
	uint64_t offset = map->arena->segs[map->arena->currSeg].offset;
	uint64_t size = map->arena->segs[map->arena->currSeg].segSize;
	uint8_t cnt = 0;

	offset += size;
	size <<= map->arena->maxDbl;

	// double the current size up to 32GB
	// with size minimum

	do size <<= 1, cnt++;
	while (size < minSize);

	if (size > 32ULL * 1024 * 1024 * 1024)
		size = 32ULL * 1024 * 1024 * 1024;
	else
		map->arena->maxDbl += cnt;

	map->arena->segs[map->arena->currSeg + 1].offset = offset;
	map->arena->segs[map->arena->currSeg + 1].segSize = size;
	map->arena->segs[map->arena->currSeg + 1].nextDoc.segment = map->arena->currSeg + 1;

	//  extend the disk file, windows does this automatically

#ifndef _WIN32
	if (map->hndl >= 0)
	  if (ftruncate(map->hndl, offset + size)) {
		fprintf (stderr, "Unable to initialize file %s, error = %d", map->fName, errno);
		return false;
	  }
#endif

	if (!mapSeg(map, map->arena->currSeg + 1))
		return false;

	map->arena->nextObject.segment = map->arena->currSeg + 1;
	map->arena->nextObject.offset = 0;
	map->arena->currSeg++;
	return true;
}

//  allocate an object
//  return 0 if out of memory.

uint64_t allocObj( DbMap* map, DbAddr *free, DbAddr *tail, int type, uint32_t size ) {
DbAddr addr;

	lockLatch(free->latch);
	addr.bits = 0;
	size += 7;
	size &= -8;

	while (!(addr.addr = getNodeFromFrame(map, free))) {
		if (!tail || !getNodeWait(map, free, tail))
			if (!initObjFrame(map, free, type, size)) {
				unlockLatch(free->latch);
				return 0;
			}
	}

	unlockLatch(free->latch);

	memset (getObj(map, addr), 0, size);
	addr.type = type;
	return addr.bits;
}

void mapAll (DbMap *map) {
	lockLatch(map->mutex);

	while (map->maxSeg < map->arena->currSeg)
		if (mapSeg (map, map->maxSeg + 1))
			map->maxSeg++;

	unlockLatch(map->mutex);
}

void* getObj(DbMap *map, DbAddr slot) {
	if (!slot.addr) {
		fprintf (stderr, "Invalid zero document ID: %s\n", map->fName);
		exit(1);
	}

	//  catch up segment mappings

	if (slot.segment > map->maxSeg)
		mapAll(map);

	return map->base[slot.segment] + slot.offset * 8ULL;
}

void closeMap(DbMap *map) {
	while (map->maxSeg)
		unmapSeg(map, map->maxSeg--);
}

//  allocate raw space in the current segment
//  or return 0 if out of memory.

uint64_t arenaAlloc(DbMap *map, uint32_t size) {
	uint64_t max, addr;

	lockLatch(map->arena->mutex);

	max = map->arena->segs[map->arena->currSeg].segSize
		  - map->arena->segs[map->arena->currSeg].nextDoc.index * sizeof(DbAddr);

	size += 7;
	size &= -8;

	// see if existing segment has space

	if (map->arena->nextObject.offset * 8ULL + size > max) {
		lockArena(map);

		if (map->arena->nextObject.offset * 8ULL + size > max) {
			if (!newSeg(map, size)) {
				unlockArena(map);
				unlockLatch (map->arena->mutex);
				return 0;
			}
		}

		unlockArena(map);
	}

	addr = map->arena->nextObject.bits;
	map->arena->nextObject.offset += size >> 3;
	unlockLatch(map->arena->mutex);
	return addr;
}

bool mapSeg (DbMap *map, uint32_t currSeg) {
	uint64_t segSize = map->arena->segs[currSeg].segSize;
	uint64_t offset = map->arena->segs[currSeg].offset;

	if ((map->base[currSeg] = mapMemory (map, offset, segSize, currSeg)))
		return true;

	return false;
}
