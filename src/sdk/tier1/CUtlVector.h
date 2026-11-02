#pragma once

#include "sdk/macros.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <span>

template <class T> struct CUtlVector {
  std::int32_t m_Size;
  PAD(0x4);
  T *m_pElements;

  std::span<T> span() {
    return {m_pElements, static_cast<std::size_t>(m_Size)};
  }
};

static_assert(offsetof(CUtlVector<int>, m_Size) == 0x0);
static_assert(offsetof(CUtlVector<int>, m_pElements) == 0x8);
