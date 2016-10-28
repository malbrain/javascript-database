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
#include "jsdb_malloc.h"

extern DbMap memMap[1];
DbMap *db;

int getPath(char *path, int off, IdEntry *entry, DbMap *parent, uint32_t segNo);
bool mapSeg (DbMap *map, uint32_t currSeg);
void mapZero(DbMap *map, uint64_t size);
void mapAll (DbMap *map);

//	find child id entry in parent LIFO list

//	call with child LIFO list locked
//	return NULL if it doesn't exist

uint64_t findId(value_t name, DbMap *parent) {
	IdEntry *idEntry;
	DbAddr slot;

	//	find new child name in parent name list

	if ((slot.bits = parent->arena->childIdRoot.bits)) do {
		idEntry = getObj (parent, slot);
		if (name.aux != idEntry->len)
			continue;
		if (memcmp (name.str, idEntry->name, name.aux))
			continue;
		if (idEntry->ref)
			break;
		return slot.bits;
	} while ((slot.bits = idEntry->next.bits));

	slot.bits = allocMap(parent, sizeof(IdEntry) + name.aux);
	idEntry = getObj(parent, slot);

	if (parent->arena->childIdRoot.addr) {
		IdEntry *idPrev = getObj(parent, parent->arena->childIdRoot);
		idEntry->id = idPrev->id + 1;
	} else
		idEntry->id = 1;

	idEntry->next.bits = parent->arena->childIdRoot.bits;
	memcpy (idEntry->name, name.str, name.aux);
	idEntry->len = name.aux;

	return slot.bits;
}

//	find existing arena or create new one

DbMap *openMap(value_t name, DbMap *parent, uint32_t baseSize, uint64_t initSize, bool onDisk) {
	IdEntry *entry;
	DbIdMap *idMap;
	DbAddr slot;
	DbMap *map;

	lockLatch(parent->arena->childIdRoot.latch);

	slot.bits = findId (name, parent);
	entry = getObj (parent, slot);

	// transfer new child maps to idMap

	lockLatch(parent->idMap.latch);

	if ((slot.bits = parent->idMap.addr)) do {
		idMap = getObj (parent, slot);
		idMap
	}

	parent->arena->childIdRoot.bits = slot.bits;	// splice onto chain head and unlock

	hndl = jsdb_alloc(sizeof(Handle), true);
	return map;
}

//  create/open a documentstore/index/database arena file
//		call with parent childId LIFO list locked

DbMap *createMap(IdEntry *idEntry, DbMap *parent, uint32_t baseSize, uint64_t initSize, bool onDisk) {
#ifdef _WIN32
	HANDLE fileHndl;
	DWORD amt = 0;
#else
	int fileHndl;
	int32_t amt = 0;
#endif
	char *path, pathBuff[MAX_path];
	DbArena *segZero = NULL;
	uint32_t segOffset;
	int pathOff;
	DbMap *map;

	// assemble file system path

	pathOff = getPath(pathBuff, sizeof(pathBuff), idEntry, parent, 0);

	if (pathOff < 0) {
		fprintf(stderr, "file path too long: %s\n", pathBuff);
		return NULL;
	}

	path = pathBuff + pathOff;

	if (onDisk) {
#ifdef _WIN32
		int flags = OPEN_ALWAYS;

		fileHndl = CreateFile (path, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, flags, FILE_ATTRIBUTE_NORMAL, NULL);
		if (fileHndl == INVALID_HANDLE_VALUE) {
			fprintf (stderr, "Unable to open/create %s, error = %d", path, (int)GetLastError());
			return NULL;
		}

		segZero = VirtualAlloc(NULL, sizeof(DbArena), MEM_COMMIT, PAGE_READWRITE);
		lockArena(fileHndl, path);

		if (!ReadFile(fileHndl, segZero, sizeof(DbArena), &amt, NULL)) {
			fprintf (stderr, "Unable to read %lld bytes from %s, error = %d", sizeof(DbArena), path, errno);
			VirtualFree(segZero, 0, MEM_RELEASE);
			unlockArena(fileHndl, path);
			CloseHandle(fileHndl);
			return NULL;
		}
#else
		int flags = O_RDWR | O_CREAT;

		fileHndl = open (path, flags, 0666);

		if (fileHndl == -1) {
			fprintf (stderr, "Unable to open/create %s, error = %d", path, errno);
			return NULL;
		}

		// read first part of segment zero if it exists

		lockArena(fileHndl, path);
		segZero = valloc(sizeof(DbArena));

		if ((amt = pread(fileHndl, segZero, sizeof(DbArena), 0))) {
			if (amt < sizeof(DbArena)) {
				fprintf (stderr, "Unable to read %d bytes from %s, error = %d", (int)sizeof(DbArena), path, errno);
				unlockArena(fileHndl, path);
				free(segZero);
				close(fileHndl);
				return NULL;
			}
		}
#endif
	} else
#ifdef _WIN32
		fileHndl = INVALID_HANDLE_VALUE;
#else
		fileHndl = -1;
#endif
	map = jsdb_alloc(sizeof(DbMap), true);
	map->cpuCount = getCpuCount();
	map->hndl[0] = fileHndl;
	map->myEntry = idEntry;
	map->onDisk = onDisk;

	//  maintain database chain

	if (parent) {
		map->parent = parent;
		map->db = parent->db;
	}

	//  if segment zero exists, map the arena

	if (amt) {
		unlockArena(fileHndl, path);
		mapZero(map, segZero->segs->size);
#ifdef _WIN32
		VirtualFree(segZero, 0, MEM_RELEASE);
		CloseHandle(fileHndl);
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
	if (onDisk && ftruncate(fileHndl, initSize)) {
		fprintf (stderr, "Unable to initialize file %s, error = %d", path, errno);
		close(fileHndl);
		return NULL;
	}
#endif

	//  initialize new arena segment zero

	mapZero(map, initSize);
	map->arena->nextObject.offset = segOffset >> 3;
	map->arena->segs->size = initSize;
	map->created = true;

	if (onDisk)
		unlockArena(fileHndl, path);
#ifdef _WIN32
	CloseHandle(fileHndl);
#endif
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

uint64_t allocObj( DbMap* map, DbAddr *free, int type, uint32_t size, bool zeroit ) {
	DbAddr slot;

	lockLatch(free->latch);
	size += 7;
	size &= -8;

	while (!(slot.bits = getNodeFromFrame(map, free))) {
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
	IdEntry *entry = map->myEntry;
	lockLatch(&map->mutex);

	while (map->maxSeg < map->arena->currSeg)
		if (mapSeg (map, map->maxSeg + 1))
			map->maxSeg++;
		else
			fprintf(stderr, "Unable to map segment %d on map %*s\n", map->maxSeg + 1, entry->len, entry->name), exit(1);

	unlockLatch(&map->mutex);
}

void* getObj(DbMap *map, DbAddr slot) {
	if (!slot.addr) {
		IdEntry *entry = map->myEntry;
		fprintf (stderr, "Invalid zero DbAddr: %*s\n", entry->len, entry->name);
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

//  initialize database container

void dbInit(bool onDisk) {
	db = createMap(NULL, NULL, 0, 0, onDisk);
}

//	create new/open exisiting Database

value_t createDatabase (value_t dbname, bool onDisk) {
	DbMap *database;
	Handle *hndl;
	value_t val;

	database = openMap(dbname, db, sizeof(DataBase), 0, onDisk);
	database->arena->idSize = sizeof(Txn);
	database->arena->type = Hndl_database;

	hndl = jsdb_alloc(sizeof(Handle), true);
	hndl->object = database;

	val.type = vt_handle;
	val.subType = Hndl_database;
	val.handle = hndl;
	return val;
}

//	allocate block in arena

uint64_t allocBlk (DbMap *map, uint32_t size) {
	uint32_t bits = MinDocType;
	DbAddr *free, slot;

	while ((1UL << bits) < size)
		bits++;

	free = &map->arena->freeBlk[bits];

	lockLatch(free->latch);
	slot.bits = bits;

	while (!(slot.addr = getNodeFromFrame(map, free))) {
		if (!initObjFrame(map, free, bits, 1UL << bits)) {
			unlockLatch(free->latch);
			return 0;
		}
	}

	unlockLatch(free->latch);
	return slot.bits;
}
