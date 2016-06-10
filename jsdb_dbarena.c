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

int getPath(char *path, int off, struct RedBlack *entry, DbMap *parent, uint32_t segNo);
bool mapSeg (DbMap *map, uint32_t currSeg);
void mapZero(DbMap *map, uint64_t size);
void mapAll (DbMap *map);

DbMap *openMap(struct RedBlack *entry, DbMap *parent, uint32_t baseSize, uint64_t initSize, bool onDisk, bool create);

value_t createMap(value_t name, DbMap *parent, uint32_t baseSize, uint64_t initSize, bool onDisk) {
	struct RedBlack *entry, *idEntry, *hndlEntry;
	uint8_t key[sizeof(ChildMap)];
	ChildIdMap *childId;
	ChildMap *child;
	Handle *hndl;
	value_t val;
	DbMap *map;

	//	add new child to parent
	//	get myId assignment.

	entry = rbAdd(parent, parent->arena->childLock, parent->arena->childRoot, name.str, name.aux, sizeof(ChildMap));

	child = (ChildMap *)(entry->key + entry->keyLen);

	//	new entry from rbAdd?

	if (!child->id)
		child->id = ++parent->arena->childId;

	store64(key, child->id);

	//  add child ID index entry

	idEntry = rbAdd(parent, parent->arena->childIdLock, parent->arena->childIdRoot, key, sizeof(key), sizeof(ChildIdMap));

	childId = (ChildIdMap *)(idEntry->key + idEntry->keyLen);
	childId->addr.bits = entry->addr.bits;

	//  add child handle entry

	hndlEntry = rbAdd(memMap, parent->hndlLock, parent->hndlTree, key, sizeof(ChildMap), sizeof(Handle));

	hndl = (Handle *)(hndlEntry->key + hndlEntry->keyLen);

	//	is this an old handle or new?

	if (!hndl->raw->addr->bits) {
		hndl->raw->addr->bits = hndlEntry->addr.bits;
		hndl->object = openMap(entry, parent, baseSize, initSize, onDisk, true);
	}

	val.bits = vt_handle;
	val.handle = hndl->raw + 1;
	val.refcount = 1;
	incrRefCnt(val);
	return val;
}

//  create/open a documentstore/index/database arena
//	call with parent's childLock set.

//  return NULL if file doesn't exist and create is not specified

DbMap *openMap(struct RedBlack *entry, DbMap *parent, uint32_t baseSize, uint64_t initSize, bool onDisk, bool create) {
#ifdef _WIN32
	HANDLE fileHndl;
#else
	int fileHndl;
#endif
	char *path, pathBuff[MAX_path];
	DbArena *segZero = NULL;
	uint32_t segOffset;
	int32_t amt = 0;
	int pathOff;
	DbMap *map;

	// assemble file system path

	pathOff = getPath(pathBuff, sizeof(pathBuff), entry, parent, 0);

	if (pathOff < 0) {
		fprintf(stderr, "file path too long: %s\n", pathBuff);
		if (parent)
			rwUnlock (parent->arena->childLock);
		exit(1);
	}

	path = pathBuff + pathOff;

	if (onDisk) {
#ifdef _WIN32
		int flags = create ? OPEN_ALWAYS : OPEN_EXISTING;

		fileHndl = CreateFile (path, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, flags, FILE_ATTRIBUTE_NORMAL, NULL);
		if (fileHndl == INVALID_HANDLE_VALUE) {
			if (parent)
				rwUnlock (parent->arena->childLock);
			if (create) {
				fprintf (stderr, "Unable to open/create %s, error = %d", path, GetLastError());
				exit(1);
			}
			return NULL;
		}

		segZero = VirtualAlloc(NULL, sizeof(DbArena), MEM_COMMIT, PAGE_READWRITE);
		lockArena(fileHndl, path);

		if (!ReadFile(fileHndl, segZero, sizeof(DbArena), &amt, NULL)) {
			fprintf (stderr, "Unable to read %lld bytes from %s, error = %d", sizeof(DbArena), path, errno);
			VirtualFree(segZero, 0, MEM_RELEASE);
			unlockArena(fileHndl, path);
			CloseHandle(fileHndl);
			if (parent)
				rwUnlock (parent->arena->childLock);
			exit(1);
		}
#else
		int flags = create ? O_RDWR : O_RDWR | O_CREAT;

		fileHndl = open (path, flags, 0666);

		if (fileHndl == -1) {
			if (parent)
				rwUnlock (parent->arena->childLock);
			if (create) {
				fprintf (stderr, "Unable to open/create %s, error = %d", path, errno);
				exit(1);
			}
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
				if (parent)
					rwUnlock (parent->arena->childLock);
				exit(1);
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
	map->onDisk = onDisk;
	map->entry = entry;

	//  maintain database chain

	if ((map->parent = parent)) {
		map->db = parent->db;
	} else
		map->db = map;

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
		if (parent)
			rwUnlock (parent->arena->childLock);
		exit(1);
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
	struct RedBlack *entry = map->entry;
	lockLatch(&map->mutex);

	while (map->maxSeg < map->arena->currSeg)
		if (mapSeg (map, map->maxSeg + 1))
			map->maxSeg++;
		else
			fprintf(stderr, "Unable to map segment %d on map %*s\n", map->maxSeg + 1, entry->keyLen, entry->key), exit(1);

	unlockLatch(&map->mutex);
}

void* getObj(DbMap *map, DbAddr slot) {
	if (!slot.addr) {
		struct RedBlack *entry = map->entry;
		fprintf (stderr, "Invalid zero DbAddr: %*s\n", entry->keyLen, entry->key);
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
	Handle *hndl;
	value_t val;

	val = createMap(dbname, memMap, sizeof(DataBase), 0, onDisk);
	val.subType = Hndl_database;

	hndl = val.handle;
	database = hndl->object;

	database->arena->idSize = sizeof(Txn);
	database->arena->type = Hndl_database;
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

//  open an arena by childId

value_t loadMap(uint64_t childId, DbMap *parent) {
	struct RedBlack *hndlEntry, *entry;
	uint8_t key[sizeof(ChildMap)];
	ChildIdMap *childMap;
	ChildMap *child;
	Handle *hndl;
	value_t val;
	DbMap *map;

	//  lookup previous arena handle
	//	or create new handle.

	store64(key, childId);
	hndlEntry = rbAdd(memMap, parent->hndlLock, parent->hndlTree, key, sizeof(ChildMap), sizeof(Handle));
	hndl = (Handle *)(hndlEntry->key + hndlEntry->keyLen);

	val.bits = vt_handle;
	val.handle = hndl->raw + 1;
	val.refcount = 1;
	incrRefCnt(val);

	rwUnlock(parent->hndlLock);

	//	is handle new?
	//	install arena map

	if (!hndl->raw->addr->bits) {
	  hndl->raw->addr->bits = hndlEntry->addr.bits;

	  writeLock(parent->arena->childIdLock);

	  //  find docStore child entry by ID

	  if ((entry = rbFind(parent, *parent->arena->childIdRoot, key, sizeof(key), NULL)))
		childMap = (ChildIdMap *)(entry->key + entry->keyLen);
	  else {
		rwUnlock(parent->arena->childIdLock);
		if (decrRefCnt(val))
			deleteValue(val);
		val.bits = vt_status;
		val.status = ERROR_handleclosed;
		return val;
	  }

	  //  find docStore entry from child

	  entry = getObj(parent, childMap->addr);
	  map = openMap(entry, parent, 0, 0, false, false);

	  //  are we in the correct database?

	  if (!map ||
	  	  map->arena->arenaGUID[0] != parent->arena->arenaGUID[0] ||
		  map->arena->arenaGUID[1] != parent->arena->arenaGUID[1]) {
			rwUnlock(parent->arena->childLock);
			if (decrRefCnt(val))
				deleteValue(val);
			val.bits = vt_status;
			val.status = ERROR_handleclosed;
			return val;
	  }

	  hndl->object = map;
	}

	if (!hndl->object) {
		val.bits = vt_status;
		val.status = ERROR_handleclosed;
		return val;
	}

	val.bits = vt_handle;
	val.handle = hndl->raw + 1;
	val.refcount = 1;
	incrRefCnt(val);

	atomicAdd64(hndl->entryCnt, 1ULL);

	if (hndl->object)
		return val;

	atomicAdd64(hndl->entryCnt, -1ULL);
	return val;
}
