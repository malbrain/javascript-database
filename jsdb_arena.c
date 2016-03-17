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

//	declare catalog map and arena

DbArena catArena[1];
DbMap catalog[1];

//
//  create/open a documentstore/index/engine arena on disk
//

DbMap *findMap(char *name, uint64_t hash, DbMap *parent);
int getPath(char *path, int off, char *fName, DbMap *parent);
uint64_t hashName(uint8_t *name, uint32_t len);
bool mapSeg (DbMap *map, uint32_t currSeg);
void mapZero(DbMap *map, uint64_t segSize);
void mapAll (DbMap *map);

DbMap* openMap(uint8_t *name, uint32_t nameLen, DbMap *parent, uint32_t baseSize, uint32_t localSize, uint64_t initSize, bool onDisk) {
#ifdef _WIN32
	HANDLE hndl;
#else
	int hndl;
#endif
	uint64_t hash = hashName(name, nameLen);
	char *fName = malloc(nameLen + 1);
	char *path, pathBuff[MAX_path];
	uint32_t segOffset;
	DbArena *segZero;
	int32_t amt = 0;
	NameList *entry;
	DbAddr child;
	int pathOff;
	DbMap *map;

	//  initialize catalog

	if (!catalog->arena)
		catalog->arena = catArena;

	//  see if Map is already open

	memcpy (fName, name, nameLen);
	fName[nameLen] = 0;

	if ((map = findMap(fName, hash, parent))) {
		free (fName);
		return map;
	}

	// assemble file system path

	pathOff = getPath(pathBuff, sizeof(pathBuff), fName, parent);

	if (pathOff < 0) {
		fprintf(stderr, "file path too long: %s\n", pathBuff);
		free (fName);
		exit(1);
	}

	path = pathBuff + pathOff;

	initSize += 4095;
	initSize &= -4096;

	if (!initSize)
		initSize = SEGZERO_size;

	if (onDisk) {
#ifdef _WIN32
		hndl = CreateFile (path, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hndl == INVALID_HANDLE_VALUE) {
			fprintf (stderr, "Unable to open/create %s, error = %d", path, GetLastError());
			free(fName);
			return NULL;
		}

		segZero = VirtualAlloc(NULL, sizeof(DbArena), MEM_COMMIT, PAGE_READWRITE);
		lockArena(hndl, path);
		if (!ReadFile(hndl, segZero, sizeof(DbArena), &amt, NULL)) {
			fprintf (stderr, "Unable to read %d bytes from %s, error = %d", sizeof(DbArena), path, errno);
			VirtualFree(segZero, 0, MEM_RELEASE);
			free(fName);
			return NULL;
		}
#else
		hndl = open (path, O_RDWR | O_CREAT, 0666);

		if (hndl == -1) {
			fprintf (stderr, "Unable to open/create %s, error = %d", path, errno);
			free(fName);
			return NULL;
		}

		// read first part of segment zero if it exists

		segZero = valloc(sizeof(DbArena));
		lockArena(hndl, path);

		if ((amt = pread(hndl, segZero, sizeof(DbArena), 0)) {
			if (amt < sizeof(DbArena)) {
				fprintf (stderr, "Unable to read %d bytes from %s, error = %d", sizeof(DbArena), path, errno);
				unlockArena(hndl, path);
				free(segZero);
				free(fName);
				return NULL;
			}
		}
#endif
	} else
#ifdef _WIN32
		hndl = INVALID_HANDLE_VALUE;
#else
		hndl = -1;
#endif

	map = jsdb_alloc(sizeof(DbMap) + localSize, true);
	map->cpuCount = getCpuCount();
	map->onDisk = onDisk;
	map->fName = fName;
	map->hash = hash;
	map->hndl = hndl;

	//	add map to parent children list

	if ((map->parent = parent)) {
		lockLatch (parent->mutex);
		map->next = parent->child;
		parent->child = map;
		unlockLatch (parent->mutex);
	}

	//  if segment zero exists, map the arena

	if (amt) {
		unlockArena(hndl, path);
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
	if (ftruncate(hndl, initSize)) {
		fprintf (stderr, "Unable to initialize file %s, error = %d", path, errno);
		exit(1);
	}
#endif

	//  initialize arena segment zero

	segOffset = sizeof(DbArena) + baseSize + sizeof(DbSeg);
	segOffset += 7;
	segOffset &= -8;

	mapZero(map, initSize);
	map->arena->nextObject.offset = segOffset;
	map->arena->segs->segSize = initSize;

	//  save segment zero data

	*(DbSeg *)(map->base) = *map->arena->segs;

	map->created = true;
	unlockArena(hndl, path);

	//  add the new child name to the parent's list

	WriteLock(parent->arena->childLock);
	child.bits = allocObj(parent, parent->arena->freeNames, NULL, ChildType, sizeof(NameList) );
	entry = getObj(parent, child);
	entry->next.bits = parent->arena->childList.bits;
	parent->arena->childList.bits = child.bits;
	WriteRelLock(parent->arena->childLock);

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
	uint32_t nextSeg = map->arena->currSeg;
	uint32_t segOffset;
	uint8_t cnt = 0;

	offset += size;
	size <<= map->arena->maxDbl;

	if (size < MIN_segsize)
		size = MIN_segsize;

	// double the current size up to 32GB
	// with size minimum

	do size <<= 1, cnt++;
	while (size < minSize);

	if (size > 32ULL * 1024 * 1024 * 1024)
		size = 32ULL * 1024 * 1024 * 1024;
	else
		map->arena->maxDbl += cnt;

	map->arena->segs[nextSeg].offset = offset;
	map->arena->segs[nextSeg].segSize = size;
	map->arena->segs[nextSeg].nextDoc.segment = nextSeg;

	//  extend the disk file, windows does this automatically

#ifndef _WIN32
	if (map->hndl >= 0)
	  if (ftruncate(map->hndl, offset + size)) {
		fprintf (stderr, "Unable to initialize file %s, error = %d", map->fName, errno);
		return false;
	  }
#endif

	if (!mapSeg(map, nextSeg))
		return false;

	//  save segment data in segment

	*(DbSeg *)(map->base[nextSeg]) = map->arena->segs[nextSeg];

	segOffset = sizeof(DbSeg);
	segOffset += 7;
	segOffset &= -8;

	map->arena->nextObject.segment = nextSeg;
	map->arena->nextObject.offset = segOffset;
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
		lockArena(map->hndl, map->fName);

		if (map->arena->nextObject.offset * 8ULL + size > max) {
			if (!newSeg(map, size)) {
				unlockArena(map->hndl, map->fName);
				unlockLatch (map->arena->mutex);
				return 0;
			}
		}

		unlockArena(map->hndl, map->fName);
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

//  assemble filename path

int getPath(char *path, int off, char *fName, DbMap *parent) {
int len;

	path[--off] = 0;
	len = strlen(fName);

	if (off > len)
		off -= len;
	else
		return -1;

	memcpy(path + off, fName, len);

	while (parent) {
		if ((len = strlen(parent->fName)))
			path[--off] = '.';
		else
			continue;

		if( off > len)
			off -= len;
		else
			return -1;

		memcpy(path + off, parent->fName, len);
	}

	len = strlen("data/");

	if (off > len)
		off -= len;
	else
		return -1;

	memcpy(path + off, "data/", len);
	return off;
}

uint64_t hashName(uint8_t *name, uint32_t len) {
    uint64_t hash = 0;
    uint64_t mask;

    while (len>=8) {
        len -= 8;
        hash += *((uint64_t *) &name[len]);
        hash *= 5;
    }

    mask = 1ULL << len * 8;
    return hash += --mask & (*((uint64_t *)name));
}

DbMap *findMap(char *name, uint64_t hash, DbMap *parent) {
	DbMap *map;
	
	lockLatch(parent->mutex);

	if ((map = parent->child)) do {
		if (map->hash == hash)
			if (!strcmp(name, parent->fName))
				break;
	} while ((map = map->next));

	unlockLatch(parent->mutex);
	return map;
}
