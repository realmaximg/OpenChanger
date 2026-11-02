#pragma once

#include "sdk/macros.h"
#include "sdk/schemasystem/SchemaBaseClassInfoData_t.h"
#include "sdk/schemasystem/SchemaClassFieldData_t.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <span>

struct SchemaClassInfoData_t {
  SchemaClassInfoData_t *m_pSelf;
  const char *m_pszName;
  PAD(0x14);
  std::uint16_t m_nFieldSize;
  PAD(0x3);
  std::uint8_t m_nBaseClassSize;
  PAD(0x6);
  SchemaClassFieldData_t *m_pFields;
  SchemaBaseClassInfoData_t *m_pBaseClasses;

  [[nodiscard]] std::span<SchemaClassFieldData_t> getFields() const {
    return {m_pFields, m_nFieldSize};
  }

  [[nodiscard]] SchemaClassInfoData_t *getBaseClass() const {
    return m_nBaseClassSize == 0 ? nullptr : m_pBaseClasses[0].m_pClass;
  }
};

static_assert(offsetof(SchemaClassInfoData_t, m_pSelf) == 0x0);
static_assert(offsetof(SchemaClassInfoData_t, m_pszName) == 0x8);
static_assert(offsetof(SchemaClassInfoData_t, m_nFieldSize) == 0x24);
static_assert(offsetof(SchemaClassInfoData_t, m_nBaseClassSize) == 0x29);
static_assert(offsetof(SchemaClassInfoData_t, m_pFields) == 0x30);
static_assert(offsetof(SchemaClassInfoData_t, m_pBaseClasses) == 0x38);
