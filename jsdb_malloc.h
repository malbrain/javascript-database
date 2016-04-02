#pragma once

//
// Reference Counting
//

typedef struct RawObj {
	uint64_t weakCnt[1];
	uint64_t refCnt[1];
	DbAddr addr[1];
} rawobj_t;

