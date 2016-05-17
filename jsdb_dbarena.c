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
#include "jsdb_db.h"
#include "jsdb_util.h"

int getPath(char *path, int off, value_t name, DbMap *parent, uint32_t segNo);
bool mapSeg (DbMap *map, uint32_t currSeg);
void mapZero(DbMap *map, uint64_t size);
void mapAll (DbMap *map);

//
//  open a documentstore/index arena
//

DbMap *openMap(value_t name, DbMap *parent) {
#ifdef _WIN32
	HANDLE hndl;
#else
	int hndl;
#endif
	char *path, pathBuff[MAX_path];
	DbArena *segZero;
	int32_t amt = 0;
	int pathOff;
	DbMap *map;

	// assemble file system path

	pathOff = getPath(pathBuff, sizeof(pathBuff), name, parent, 0);

	if (pathOff < 0) {
		fprintf(stderr, "file path too long: %s\n", pathBuff);
		exit(1);
	}

	path = pathBuff + pathOff;

#ifdef _WIN32
	hndl = CreateFile (path, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hndl == INVALID_HANDLE_VALUE)
		return NULL;

	segZero = VirtualAlloc(NULL, sizeof(DbArena), MEM_COMMIT, PAGE_READWRITE);
	lockArena(hndl, path);

	if (!ReadFile(hndl, segZero, sizeof(DbArena), &amt, NULL) || amt < sizeof(DbArena)) {
		fprintf (stderr, "Unable to read %lld bytes from %s, error = %d", sizeof(DbArena), path, errno);
		VirtualFree(segZero, 0, MEM_RELEASE);
		unlockArena(hndl, path);
		CloseHandle(hndl);
		return NULL;
	}
#else
	hndl = open (path, O_RDWR, 0666);

	if (hndl == -1)
		return NULL;

	// read first part of segment zero if it exists

	segZero = valloc(sizeof(DbArena));
	lockArena(hndl, path);

	if (pread(hndl, segZero, sizeof(DbArena), 0) < sizeof(DbArena)) {
		fprintf (stderr, "Unable to read %d bytes from %s, error = %d", (int)sizeof(DbArena), path, errno);
		unlockArena(hndl, path);
		free(segZero);
		close(hndl);
		return NULL;
	}
#endif

	map = jsdb_alloc(sizeof(DbMap), true);
	map->cpuCount = getCpuCount();
	map->parent = parent;
	map->hndl[0] = hndl;
	map->onDisk = true;
	map->name = name;

	//  map the arena

	unlockArena(hndl, path);
	mapZero(map, segZero->segs->size);
#ifdef _WIN32
	VirtualFree(segZero, 0, MEM_RELEASE);
	CloseHandle(hndl);
#else
	free(segZero);
#endif
	// wait for initialization to finish

	waitNonZero(&map->arena->type);
	return map;
}

//
//  create/open a documentstore/index/engine arena on disk
//

DbMap* createMap(value_t name, DbMap *parent, uint32_t baseSize, uint64_t initSize, bool onDisk) {
#ifdef _WIN32
	HANDLE hndl;
#else
	int hndl;
#endif
	char *path, pathBuff[MAX_path];
	DbArena *segZero = NULL;
	uint32_t segOffset;
	int32_t amt = 0;
	NameList *entry;
	DbAddr child;
	int pathOff;
	DbMap *map;

	// assemble file system path

	pathOff = getPath(pathBuff, sizeof(pathBuff), name, parent, 0);

	if (pathOff < 0) {
		fprintf(stderr, "file path too long: %s\n", pathBuff);
		exit(1);
	}

	path = pathBuff + pathOff;

	if (onDisk) {
#ifdef _WIN32
		hndl = CreateFile (path, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hndl == INVALID_HANDLE_VALUE) {
			fprintf (stderr, "Unable to open/create %s, error = %d", path, GetLastError());
			return NULL;
		}

		segZero = VirtualAlloc(NULL, sizeof(DbArena), MEM_COMMIT, PAGE_READWRITE);
		lockArena(hndl, path);

		if (!ReadFile(hndl, segZero, sizeof(DbArena), &amt, NULL)) {
			fprintf (stderr, "Unable to read %lld bytes from %s, error = %d", sizeof(DbArena), path, errno);
			VirtualFree(segZero, 0, MEM_RELEASE);
			unlockArena(hndl, path);
			CloseHandle(hndl);
			return NULL;
		}
#else
		hndl = open (path, O_RDWR | O_CREAT, 0666);

		if (hndl == -1) {
			fprintf (stderr, "Unable to open/create %s, error = %d", path, errno);
			return NULL;
		}

		// read first part of segment zero if it exists

		lockArena(hndl, path);
		segZero = valloc(sizeof(DbArena));

		if ((amt = pread(hndl, segZero, sizeof(DbArena), 0))) {
			if (amt < sizeof(DbArena)) {
				fprintf (stderr, "Unable to read %d bytes from %s, error = %d", (int)sizeof(DbArena), path, errno);
				unlockArena(hndl, path);
				free(segZero);
				close(hndl);
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

	map = jsdb_alloc(sizeof(DbMap), true);
	map->cpuCount = getCpuCount();
	map->onDisk = onDisk;
	map->parent = parent;
	map->hndl[0] = hndl;
	map->name = name;

	//  if segment zero exists, map the arena

	if (amt) {
		unlockArena(hndl, path);
		mapZero(map, segZero->segs->size);
#ifdef _WIN32
		VirtualFree(segZero, 0, MEM_RELEASE);
		CloseHandle(hndl);
#else
		free(segZero);
#endif
		// wait for initialization to finish

		waitNonZero(&map->arena->type);
		return map;
	}

	if (segZero) {
#ifdef _WIN32
		VirtualFree(segZero, 0, MEM_RELEASE);
#else
		free(segZero);
#endif
	}

	segOffset = sizeof(DbArena) + baseSize;
	segOffset += 7;
	segOffset &= -8;

	if (initSize < segOffset)
		initSize = segOffset;

	if (initSize < MIN_segsize)
		initSize = MIN_segsize;

	initSize += 65535;
	initSize &= -65536;

	//  create initial segment on unix, windows will automatically do it

#ifndef _WIN32
	if (onDisk && ftruncate(hndl, initSize)) {
		fprintf (stderr, "Unable to initialize file %s, error = %d", path, errno);
		close(hndl);
		exit(1);
	}
#endif

	//  initialize new arena segment zero

	mapZero(map, initSize);
	map->arena->nextObject.offset = segOffset >> 3;
	map->arena->segs->size = initSize;

	map->created = true;

	if (onDisk)
		unlockArena(hndl, path);

#ifdef _WIN32
	CloseHandle(hndl);
#endif
	if (!parent)
		return map;

	//  add the new child to the parent's child name list

	writeLock(parent->arena->childLock);
	child.bits = allocMap(parent, sizeof(NameList) + name.aux);
	child.type = ChildType;

	entry = getObj(parent, child);
	entry->seq = ++parent->arena->childSeq;
	entry->next.bits = parent->arena->childList.bits;

	memcpy (entry->name, name.str, name.aux);

	parent->arena->childList.bits = child.bits;
	parent->arena->childCnt++;
	rwUnlock(parent->arena->childLock);

	return map;
}

//  initialize arena segment zero

void mapZero(DbMap *map, uint64_t size) {

	map->arena = mapMemory (map, 0, size, 0);
	map->base[0] = (char *)map->arena;

	mapAll(map);
}

//  extend arena into new segment
//  return FALSE if out of memory

bool newSeg(DbMap *map, uint32_t minSize) {
	uint64_t off = map->arena->segs[map->arena->currSeg].off;
	uint64_t size = map->arena->segs[map->arena->currSeg].size;
	uint32_t nextSeg = map->arena->currSeg + 1;
	uint8_t cnt = 0;

	minSize += sizeof(DbSeg);

	// bootstrapping new inMem arena?

	if (size)
		off += size;
	else
		nextSeg = 0;

	if (size < MIN_segsize / 2)
		size = MIN_segsize / 2;

	// double the current size up to 32GB

	do size <<= 1;
	while (size < minSize);

	if (size > 32ULL * 1024 * 1024 * 1024)
		size = 32ULL * 1024 * 1024 * 1024;

	map->arena->segs[nextSeg].off = off;
	map->arena->segs[nextSeg].size = size;
	map->arena->segs[nextSeg].nextDoc.segment = nextSeg;

	//  extend the disk file, windows does this automatically

#ifndef _WIN32
	if (map->hndl[0] >= 0)
	  if (ftruncate(map->hndl[0], off + size)) {
		fprintf (stderr, "Unable to initialize file %s, error = %d", map->name.str, errno);
		return false;
	  }
#endif

	if (!mapSeg(map, nextSeg))
		return false;

	map->maxSeg = nextSeg;

	map->arena->nextObject.offset = nextSeg ? 0 : 1;
	map->arena->nextObject.segment = nextSeg;
	map->arena->currSeg = nextSeg;
	return true;
}

//  allocate an object
//  return 0 if out of memory.

uint64_t allocObj( DbMap* map, DbAddr *free, DbAddr *tail, int type, uint32_t size, bool zeroit ) {
	DbAddr slot;

	lockLatch(free->latch);
	slot.bits = 0;
	size += 7;
	size &= -8;

	while (!(slot.addr = getNodeFromFrame(map, free))) {
		if (!tail || !getNodeWait(map, free, tail))
			if (!initObjFrame(map, free, type, size)) {
				unlockLatch(free->latch);
				return 0;
			}
	}

	unlockLatch(free->latch);

	if (zeroit)
		memset (getObj(map, slot), 0, size);

	{
		uint64_t max = map->arena->segs[slot.segment].size
		  - map->arena->segs[slot.segment].nextDoc.index * sizeof(DbAddr);

		if (slot.offset * 8ULL + size > max)
			fprintf(stderr, "allocObj segment overrun\n"), exit(1);
	}

	slot.type = type;
	return slot.bits;
}

void mapAll (DbMap *map) {
	lockLatch(&map->mutex);

	while (map->maxSeg < map->arena->currSeg)
		if (mapSeg (map, map->maxSeg + 1))
			map->maxSeg++;
		else
			fprintf(stderr, "Unable to map segment %d on map %s\n", map->maxSeg + 1, map->name.str), exit(1);

	unlockLatch(&map->mutex);
}

void* getObj(DbMap *map, DbAddr slot) {
	if (!slot.addr) {
		fprintf (stderr, "Invalid zero document ID: %s\n", map->name.str);
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

uint64_t allocMap(DbMap *map, uint32_t size) {
	uint64_t max, addr;

	lockLatch(&map->arena->mutex);

	max = map->arena->segs[map->arena->currSeg].size
		  - map->arena->segs[map->arena->currSeg].nextDoc.index * map->arena->idSize;

	size += 7;
	size &= -8;

	// see if existing segment has space
	// otherwise allocate a new segment.

	if (map->arena->nextObject.offset * 8ULL + size > max) {
		if (!newSeg(map, size)) {
			unlockLatch (&map->arena->mutex);
			return 0;
		}
	}

	addr = map->arena->nextObject.bits;
	map->arena->nextObject.offset += size >> 3;
	unlockLatch(&map->arena->mutex);
	return addr;
}

bool mapSeg (DbMap *map, uint32_t currSeg) {
	uint64_t size = map->arena->segs[currSeg].size;
	uint64_t off = map->arena->segs[currSeg].off;

	if ((map->base[currSeg] = mapMemory (map, off, size, currSeg)))
		return true;

	return false;
}

value_t createDatabase (value_t dbname, bool onDisk) {
	value_t v, n, val = newObject();
	DbMap *database;

	database = createMap(dbname, NULL, sizeof(DataBase), 0, onDisk);
	database->arena->idSize = sizeof(Txn);
	database->arena->type = hndl_database;

	v.bits = vt_handle;
	v.aux = hndl_database;
	v.hndl = database;
	v.refcount = 1;

	n.bits = vt_string;
	n.string = "_dbhndl";
	n.aux = 7;

	incrRefCnt(v);
	*lookup(val.oval, n, true) = v;
	return val;
}

