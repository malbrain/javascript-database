#pragma once

//
// Reference Counting
//

typedef struct RawObj {
	int64_t weakCnt[1];
	int64_t refCnt[1];
	uint64_t addr;
} rawobj_t;

uint64_t js_rawAlloc(uint32_t len, bool zeroit);
