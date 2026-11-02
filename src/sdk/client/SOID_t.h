#pragma once

#include "sdk/macros.h"

#include <array>
#include <cstddef>
#include <cstdint>

namespace GCSDK {

struct SOID_t {
  std::uint64_t m_id;
  std::uint32_t m_type;
  PAD(0x4);
};

static_assert(sizeof(SOID_t) == 0x10);
static_assert(offsetof(SOID_t, m_id) == 0x0);
static_assert(offsetof(SOID_t, m_type) == 0x8);

} // namespace GCSDK
