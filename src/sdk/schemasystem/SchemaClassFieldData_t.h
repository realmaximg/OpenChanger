#pragma once

#include "sdk/macros.h"

#include <array>
#include <cstddef>
#include <cstdint>

struct SchemaClassFieldData_t {
  const char *m_pszName;
  PAD(0x8);
  std::uint32_t m_nSingleInheritanceOffset;
  PAD(0xc);
};

static_assert(offsetof(SchemaClassFieldData_t, m_pszName) == 0x0);
static_assert(offsetof(SchemaClassFieldData_t, m_nSingleInheritanceOffset) ==
              0x10);
static_assert(sizeof(SchemaClassFieldData_t) == 0x20);
