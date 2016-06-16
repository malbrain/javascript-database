#pragma once

//
// Reference Counting
//

typedef struct RawObj {
	int64_t weakCnt[1];
	int64_t refCnt[1];
	DbAddr addr[1];
} rawobj_t;

