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
#include "jsdb_redblack.h"

int getPath(char *path, int off, value_t name, DbMap *parent, uint32_t segNo);
bool mapSeg (DbMap *map, uint32_t currSeg);
void mapZero(DbMap *map, uint64_t size);
void mapAll (DbMap *map);

//
//  open a documentstore/index arena
//

DbMap *openMap(value_t name, DbMap *parent, uint32_t hndlIdx) {
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

	incrHndlCnt(&parent->hndls.aval->hndls[hndlIdx]);
	
	if ((map = parent->hndls.aval->hndls[hndlIdx].object))
		return map;

	// assemble file system path

	pathOff = getPath(pathBuff, sizeof(pathBuff), name, parent, 0);

	if (pathOff < 0) {
		fprintf(stderr, "file path too long: %s\n", pathBuff);
		decrHndlCnt(&parent->hndls.aval->hndls[hndlIdx]);
		exit(1);
	}

	path = pathBuff + pathOff;

#ifdef _WIN32
	hndl = CreateFile (path, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hndl == INVALID_HANDLE_VALUE) {
		decrHndlCnt(&parent->hndls.aval->hndls[hndlIdx]);
		return NULL;
	}

	segZero = VirtualAlloc(NULL, sizeof(DbArena), MEM_COMMIT, PAGE_READWRITE);
	lockArena(hndl, path);

	if (!ReadFile(hndl, segZero, sizeof(DbArena), &amt, NULL) || amt < sizeof(DbArena)) {
		fprintf (stderr, "Unable to read %lld bytes from %s, error = %d", sizeof(DbArena), path, errno);
		decrHndlCnt(&parent->hndls.aval->hndls[hndlIdx]);
		VirtualFree(segZero, 0, MEM_RELEASE);
		unlockArena(hndl, path);
		CloseHandle(hndl);
		return NULL;
	}
#else
	hndl = open (path, O_RDWR, 0666);

	if (hndl == -1) {
		decrHndlCnt(&parent->hndls.aval->hndls[hndlIdx]);
		return NULL;
	}

	// read first part of segment zero if it exists

	segZero = valloc(sizeof(DbArena));
	lockArena(hndl, path);

	if (pread(hndl, segZero, sizeof(DbArena), 0) < sizeof(DbArena)) {
		fprintf (stderr, "Unable to read %d bytes from %s, error = %d", (int)sizeof(DbArena), path, errno);
		decrHndlCnt(&parent->hndls.aval->hndls[hndlIdx]);
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
	parent->hndls.aval->hndls[hndlIdx].object = map;
	return map;
}

//  find/add child to the parent's child list
//	call with parent arena child lock set.

uint32_t findChild (DbMap *parent, value_t name) {
	DbAddr child, prev;
	PathStk path[1];
	RedBlack *entry;

	readLock(parent->arena->childLock);

	if ((child.bits = rbFind(parent, name.str, name.aux, path))) {
		entry = getObj(parent, child);
		incrHndlCnt(&parent->hndls.aval->hndls[entry->hndlIdx]);
		rwUnlock(parent->arena->childLock);
		return entry->hndlIdx;
	}

	if ((child.bits = allocBlk(parent, sizeof(RedBlack) + name.aux)))
		entry = getObj(parent, child);
		memcpy (entry->name, name.str, name.aux);
		entry->nameLen = name.aux;

		//  remove top entry of waiting hndl holders

		if ((prev.bits = parent->arena->childSlots.bits)) {
			RedBlack *head = getObj(parent, parent->arena->childSlots);
			parent->arena->childSlots.bits = head->next.bits;
			entry->hndlIdx = head->hndlIdx;

			//  done with the hndl holder, return space to arena

			if (!addSlotToFrame(parent, &parent->arena->freeBlk[prev.type], prev.bits)) {
				rwUnlock(parent->arena->childLock);
				return 0;
			}
		} else {
			entry->hndlIdx = ++parent->arena->childIdx;

		//	add new entry to the red/black tree

		rbInsert (parent, child, path);
	}

	incrHndlCnt(&parent->hndls.aval->hndls[entry->hndlIdx]);
	rwUnlock(parent->arena->childLock);
	return entry->hndlIdx;
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
	uint32_t segOffset, hndlIdx = 0;
	char *path, pathBuff[MAX_path];
	DbArena *segZero = NULL;
	int32_t amt = 0;
	int pathOff;
	DbMap *map;

	if (parent) {
		hndlIdx = findChild(parent, name);
	}

	// assemble file system path

	pathOff = getPath(pathBuff, sizeof(pathBuff), name, parent, 0);

	if (pathOff < 0) {
		fprintf(stderr, "file path too long: %s\n", pathBuff);
		if (parent)
			decrHndlCnt(&parent->hndls.aval->hndls[hndlIdx]);
		exit(1);
	}

	path = pathBuff + pathOff;

	if (onDisk) {
#ifdef _WIN32
		hndl = CreateFile (path, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hndl == INVALID_HANDLE_VALUE) {
			fprintf (stderr, "Unable to open/create %s, error = %d", path, GetLastError());
			if (parent)
				decrHndlCnt(&parent->hndls.aval->hndls[hndlIdx]);
			return NULL;
		}

		segZero = VirtualAlloc(NULL, sizeof(DbArena), MEM_COMMIT, PAGE_READWRITE);
		lockArena(hndl, path);

		if (!ReadFile(hndl, segZero, sizeof(DbArena), &amt, NULL)) {
			fprintf (stderr, "Unable to read %lld bytes from %s, error = %d", sizeof(DbArena), path, errno);
			VirtualFree(segZero, 0, MEM_RELEASE);
			unlockArena(hndl, path);
			CloseHandle(hndl);
			if (parent)
				decrHndlCnt(&parent->hndls.aval->hndls[hndlIdx]);
			return NULL;
		}
#else
		hndl = open (path, O_RDWR | O_CREAT, 0666);

		if (hndl == -1) {
			fprintf (stderr, "Unable to open/create %s, error = %d", path, errno);
			if (parent)
				decrHndlCnt(&parent->hndls.aval->hndls[hndlIdx]);
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
				if (parent)
					decrHndlCnt(&parent->hndls.aval->hndls[hndlIdx]);
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
	map->hndl[0] = hndl;
	map->name = name;

	//  maintain database chain

	if ((map->parent = parent))
		map->db = parent->db;
	else
		map->db = map;

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
		if (parent)
			parent->hndls.aval->hndls[hndlIdx].object = map;
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
		if (parent)
			decrHndlCnt(&parent->hndls.aval->hndls[hndlIdx]);
		exit(1);
	}
#endif

	//  initialize new arena segment zero

	mapZero(map, initSize);
	map->arena->nextObject.offset = segOffset >> 3;
	map->arena->segs->size = initSize;
	map->arena->hndlIdx = hndlIdx;

	map->created = true;

	if (onDisk)
		unlockArena(hndl, path);

#ifdef _WIN32
	CloseHandle(hndl);
#endif
	if (parent)
		parent->hndls.aval->hndls[hndlIdx].object = map;

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

uint64_t allocObj( DbMap* map, DbAddr *free, int type, uint32_t size, bool zeroit ) {
	DbAddr slot;

	lockLatch(free->latch);
	slot.bits = type;
	size += 7;
	size &= -8;

	while (!(slot.addr = getNodeFromFrame(map, free))) {
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
	DbMap *database;
	value_t val;

	database = createMap(dbname, NULL, sizeof(DataBase), 0, onDisk);
	database->arena->idSize = sizeof(Txn);
	database->arena->type = Hndl_database;

	//  open objects to track collections 

	database->hndls = newArray(array_handle);
	val.bits = vt_handle;
	val.subType = Hndl_database;
	val.hndl = database->hndls.aval;
	val.refcount = 1;
	incrRefCnt(val);

	vec_push (val.hndl->values, val);
	return val;
}

//	allocate block in arena

uint64_t allocBlk (DbMap *map, uint32_t size) {
	uint32_t bits = MinDocType;
	DbAddr *free, slot;
	Status stat;

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

void *lockHandle(value_t val) {
	handle_t *hndl = val.hndl->hndls + val.aux;

	if (!hndl->object)
		return NULL;

	incrHndlCnt(hndl);

	if (!hndl->object)
		decrHndlCnt(hndl);

	return hndl->object;
}

void unlockHandle(value_t val) {
	handle_t *hndl = val.hndl->hndls + val.aux;

	decrHndlCnt(hndl);
}
