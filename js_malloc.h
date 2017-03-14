#pragma once

//
// Reference Counting
//

typedef struct RawObj {
	uint32_t weakCnt[1];
	uint32_t refCnt[1];
	uint64_t addr[1];
} rawobj_t;

