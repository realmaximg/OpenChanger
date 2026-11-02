#pragma once

#include <cstdint>

struct CPlayerSlot {
  std::int32_t m_Data;
};

static_assert(sizeof(CPlayerSlot) == 0x4);
