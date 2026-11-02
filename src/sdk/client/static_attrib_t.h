#pragma once

#include "sdk/macros.h"

#include <array>
#include <cstddef>
#include <cstdint>

struct static_attrib_t {
  std::uint16_t iDefIndex;
  PAD(0x6);
  std::uint32_t m_value;
  PAD(0xc);
};

static_assert(sizeof(static_attrib_t) == 0x18);
static_assert(offsetof(static_attrib_t, iDefIndex) == 0x0);
static_assert(offsetof(static_attrib_t, m_value) == 0x8);
