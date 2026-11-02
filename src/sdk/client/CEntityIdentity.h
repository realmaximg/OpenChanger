#pragma once

#include "sdk/macros.h"

#include <array>
#include <cstddef>
#include <cstdint>

class CEntityInstance;

struct CEntityIdentity {
  CEntityInstance *m_pInstance;
  PAD(0x8);
  std::uint32_t m_EHandle;
  PAD(0x5c);
};

static_assert(offsetof(CEntityIdentity, m_pInstance) == 0x0);
static_assert(offsetof(CEntityIdentity, m_EHandle) == 0x10);
static_assert(sizeof(CEntityIdentity) == 0x70);
