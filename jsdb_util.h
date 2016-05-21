#pragma once

#define MUTEX_BIT  0x1
#define DEAD_BIT   0x2

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#define relax() asm volatile("pause\n" : : : "memory")
#endif

/**
 * spin latches
 */

void lockLatch(volatile char* latch);
void unlockLatch(volatile char* latch);
void waitNonZero(volatile char *zero);
void art_yield();

/**
 * atomic integer ops
 */

void kill_slot(volatile char* latch);

uint64_t atomicAdd64(volatile uint64_t *value, uint64_t amt);
uint32_t atomicAdd32(volatile uint32_t *value, uint32_t amt);
uint64_t atomicOr64(volatile uint64_t *value, uint64_t amt);
uint32_t atomicOr32(volatile uint32_t *value, uint32_t amt);
uint64_t CompareAndSwap(uint64_t* target, uint64_t compare_val, uint64_t swap_val);

/**
 *  memory mapping
 */

void* mapMemory(DbMap *map, uint64_t offset, uint64_t size, uint32_t segNo);
void unmapSeg(DbMap *map, uint32_t segNo);
bool mapSeg(DbMap *map, uint32_t segNo);

/**
 *  interface to 'rdtscp', when available
 */

uint32_t getCpuCount();
uint32_t getSet();

/**
 *	file system lock
 */

#ifdef _WIN32
void lockArena(HANDLE hndl, char *fName);
void unlockArena(HANDLE hndl, char *fName);
#else
void lockArena(int hndl, char *fName);
void unlockArena(int hndl, char *fName);
#endif

bool fileExists(char *path);
