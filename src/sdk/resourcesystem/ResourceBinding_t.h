#pragma once

#include <cstddef>

struct ResourceBinding_t {
  void *m_pData;
};

static_assert(offsetof(ResourceBinding_t, m_pData) == 0x0);
