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

//	declare catalog map and arena

DbArena catArena[1];
DbMap catalog[1];

DbMap *findMap(char *name, uint64_t hash, DbMap *parent);
int getPath(char *path, int off, char *fName, DbMap *parent);
bool mapSeg (DbMap *map, uint32_t currSeg);
void mapZero(DbMap *map, uint64_t size);
void mapAll (DbMap *map);

//
//  find a documentstore/index/engine arena or open on disk
//

DbMap* openMap(value_t name, DbMap *parent) {
#ifdef _WIN32
	HANDLE hndl;
#else
	int hndl;
#endif
	char *fName = malloc(name.aux + 1);
	char *path, pathBuff[MAX_path];
	uint64_t hash = hashStr(name);
	DbArena *segZero;
	int32_t amt = 0;
	DbAddr child;
	int pathOff;
	DbMap *map;

	//  initialize catalog

	if (!catalog->arena) {
		catalog->arena = catArena;
#ifdef _WIN32
		catalog->hndl = INVALID_HANDLE_VALUE;
#else
		catalog->hndl = -1;
#endif
	}

	//  see if Map is already open

	memcpy (fName, name.str, name.aux);
	fName[name.aux] = 0;

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

#ifdef _WIN32
	hndl = CreateFile (path, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hndl == INVALID_HANDLE_VALUE) {
		free(fName);
		return NULL;
	}

	segZero = VirtualAlloc(NULL, sizeof(DbArena), MEM_COMMIT, PAGE_READWRITE);
	lockArena(hndl, path);

	if (!ReadFile(hndl, segZero, sizeof(DbArena), &amt, NULL) || amt < sizeof(DbArena)) {
		fprintf (stderr, "Unable to read %d bytes from %s, error = %d", sizeof(DbArena), path, errno);
		VirtualFree(segZero, 0, MEM_RELEASE);
		unlockArena(hndl, path);
		free(fName);
		return NULL;
	}
#else
	hndl = open (path, O_RDWR, 0666);

	if (hndl == -1) {
		free(fName);
		return NULL;
	}

	// read first part of segment zero if it exists

	segZero = valloc(sizeof(DbArena));
	lockArena(hndl, path);

	if (pread(hndl, segZero, sizeof(DbArena), 0) < sizeof(DbArena)) {
		fprintf (stderr, "Unable to read %d bytes from %s, error = %d", sizeof(DbArena), path, errno);
		unlockArena(hndl, path);
		free(segZero);
		free(fName);
		return NULL;
	}
#endif

	map = jsdb_alloc(sizeof(DbMap) + segZero->localSize, true);
	map->cpuCount = getCpuCount();
	map->fName = fName;
	map->hash = hash;
	map->hndl = hndl;

	//	add map to parent children list

	if ((map->parent = parent)) {
		lockLatch (&parent->mutex);
		map->next = parent->child;
		parent->child = map;
		unlockLatch (&parent->mutex);
	}

	//  map the arena

	unlockArena(hndl, path);
	mapZero(map, segZero->segs->size);
#ifdef _WIN32
	VirtualFree(segZero, 0, MEM_RELEASE);
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

DbMap* createMap(value_t name, DbMap *parent, uint32_t baseSize, uint32_t localSize, uint64_t initSize, bool onDisk) {
#ifdef _WIN32
	HANDLE hndl;
#else
	int hndl;
#endif
	char *fName = malloc(name.aux + 1);
	char *path, pathBuff[MAX_path];
	uint64_t hash = hashStr(name);
	uint32_t segOffset;
	DbArena *segZero;
	int32_t amt = 0;
	NameList *entry;
	DbAddr child;
	int pathOff;
	DbMap *map;

	//  initialize catalog

	if (!catalog->arena) {
		catalog->arena = catArena;
#ifdef _WIN32
		catalog->hndl = INVALID_HANDLE_VALUE;
#else
		catalog->hndl = -1;
#endif
	}

	//  see if Map is already open

	memcpy (fName, name.str, name.aux);
	fName[name.aux] = 0;

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
			unlockArena(hndl, path);
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
		lockLatch (&parent->mutex);
		map->next = parent->child;
		parent->child = map;
		unlockLatch (&parent->mutex);
	}

	//  if segment zero exists, map the arena

	if (amt) {
		unlockArena(hndl, path);
		mapZero(map, segZero->segs->size);
#ifdef _WIN32
		VirtualFree(segZero, 0, MEM_RELEASE);
#else
		free(segZero);
#endif
		// wait for initialization to finish

		waitNonZero(&map->arena->type);
		return map;
	}

#ifdef _WIN32
	VirtualFree(segZero, 0, MEM_RELEASE);
#else
	free(segZero);
#endif

	//  create initial segment on unix, windows will automatically do it

#ifndef _WIN32
	if (onDisk && ftruncate(hndl, initSize)) {
		fprintf (stderr, "Unable to initialize file %s, error = %d", path, errno);
		exit(1);
	}
#endif

	//  initialize arena segment zero

	segOffset = sizeof(DbArena) + baseSize + sizeof(DbSeg);
	segOffset += 7;
	segOffset &= -8;

	if (initSize < segOffset)
		initSize = segOffset;

	if (initSize < MIN_segsize)
		initSize = MIN_segsize;

	initSize += 65535;
	initSize &= -65536;

	mapZero(map, initSize);
	map->arena->nextObject.offset = segOffset >> 3;
	map->arena->segs->size = initSize;
	map->arena->localSize = localSize;

	//  save segment zero data

	*(DbSeg *)(map->base[0]) = map->arena->segs[0];
	map->created = true;

	if (onDisk)
		unlockArena(hndl, path);

	//  add the new child name to the parent's list

	writeLock(parent->arena->childLock);
	child.bits = allocMap(parent, sizeof(NameList) + name.aux);
	child.type = ChildType;
	entry = getObj(parent, child);
	entry->seq = ++parent->arena->childSeq;
	entry->next.bits = parent->arena->childList.bits;
	parent->arena->childList.bits = child.bits;
	memcpy (entry->name, name.str, name.aux);
	entry->name[name.aux] = 0;
	entry->len = name.aux;
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
	uint32_t segOffset;
	uint8_t cnt = 0;

	off += size;
	size <<= map->arena->maxDbl;

	if (size < MIN_segsize / 2)
		size = MIN_segsize / 2;

	// double the current size up to 32GB
	// with size minimum

	do size <<= 1, cnt++;
	while (size < minSize);

	if (size > 32ULL * 1024 * 1024 * 1024)
		size = 32ULL * 1024 * 1024 * 1024;
	else
		map->arena->maxDbl += cnt;

	map->arena->segs[nextSeg].off = off;
	map->arena->segs[nextSeg].size = size;
	map->arena->segs[nextSeg].nextDoc.segment = nextSeg;

	//  extend the disk file, windows does this automatically

#ifndef _WIN32
	if (map->hndl >= 0)
	  if (ftruncate(map->hndl, off + size)) {
		fprintf (stderr, "Unable to initialize file %s, error = %d", map->fName, errno);
		return false;
	  }
#endif

	if (!mapSeg(map, nextSeg))
		return false;

	map->maxSeg = nextSeg;

	//  save segment data in segment

	*(DbSeg *)(map->base[nextSeg]) = map->arena->segs[nextSeg];

	segOffset = sizeof(DbSeg);
	segOffset += 7;
	segOffset &= -8;

	map->arena->nextObject.segment = nextSeg;
	map->arena->nextObject.offset = segOffset >> 3;
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
	lockLatch(&map->mutex);

	while (map->maxSeg < map->arena->currSeg)
		if (mapSeg (map, map->maxSeg + 1))
			map->maxSeg++;
		else
			fprintf(stderr, "Unable to map segment %d on map %s\n", map->maxSeg + 1, map->fName), exit(1);

	unlockLatch(&map->mutex);
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

uint64_t allocMap(DbMap *map, uint32_t size) {
	uint64_t max, addr;

	lockLatch(&map->arena->mutex);

	max = map->arena->segs[map->arena->currSeg].size
		  - map->arena->segs[map->arena->currSeg].nextDoc.index * sizeof(DbAddr);

	size += 7;
	size &= -8;

	// see if existing segment has space

	if (map->arena->nextObject.offset * 8ULL + size > max) {
		if (map->arena->nextObject.offset * 8ULL + size > max) {
			if (!newSeg(map, size)) {
				unlockLatch (&map->arena->mutex);
				return 0;
			}
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
		if (parent->fName)
			len = strlen(parent->fName);
		else
			break;;

		if (len)
			path[--off] = '.';
		else
			break;

		if( off > len)
			off -= len;
		else
			return -1;

		memcpy(path + off, parent->fName, len);
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

DbMap *findMap(char *name, uint64_t hash, DbMap *parent) {
	DbMap *map;
	
	lockLatch(&parent->mutex);

	if ((map = parent->child)) do {
		if (map->hash == hash)
			if (!strcmp(name, parent->fName))
				break;
	} while ((map = map->next));

	unlockLatch(&parent->mutex);
	return map;
}
