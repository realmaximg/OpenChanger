#pragma once

#include "sdk/macros.h"

#include <array>
#include <cstddef>
#include <cstdint>

struct SchemaClassInfoData_t;

struct SchemaBaseClassInfoData_t {
  std::uint32_t m_unOffset;
  PAD(0x4);
  SchemaClassInfoData_t *m_pClass;
};

static_assert(offsetof(SchemaBaseClassInfoData_t, m_unOffset) == 0x0);
static_assert(offsetof(SchemaBaseClassInfoData_t, m_pClass) == 0x8);
static_assert(sizeof(SchemaBaseClassInfoData_t) == 0x10);
