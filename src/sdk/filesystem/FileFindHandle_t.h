#pragma once

#include "sdk/macros.h"

#include <array>

struct alignas(8) FileFindHandle_t {
  PAD(0x10);
};

static_assert(sizeof(FileFindHandle_t) == 0x10);
