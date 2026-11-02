#pragma once

#include <cstddef>

struct CUtlString {
  const char *m_pString;

  [[nodiscard]] const char *get() const {
    return m_pString != nullptr ? m_pString : "";
  }
};

static_assert(sizeof(CUtlString) == 0x8);
static_assert(offsetof(CUtlString, m_pString) == 0x0);
