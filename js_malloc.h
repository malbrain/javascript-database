#pragma once

//
// Reference Counting
//

typedef struct RawObj {
	int32_t weakCnt[1];
	int32_t refCnt[1];
	uint64_t addr[1];
} rawobj_t;

