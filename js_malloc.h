#pragma once

#include "database/db.h"

//
// Reference Counting
//

typedef struct RawObj {
  uint32_t weakCnt[1];
  uint32_t refCnt[1];
} rawobj_t;

