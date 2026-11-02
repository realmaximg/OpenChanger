#pragma once

#include "sdk/macros.h"

#include <cstddef>

struct tier0 {
  SDK_CLASS(tier0);

  SDK_EXPORT_FUNCTION(Tier0, void *(std::size_t), MemAlloc_AllocFunc,
                      "MemAlloc_AllocFunc");
};
