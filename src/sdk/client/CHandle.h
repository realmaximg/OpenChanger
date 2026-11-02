#pragma once

#include "sdk/client/CGameEntitySystem.h"

#include <cstdint>

template <class T> struct CHandle {
  std::uint32_t m_Handle;

  [[nodiscard]] T *get() const {
    CGameEntitySystem *const System = CGameEntitySystem::get();
    if (System == nullptr) {
      return nullptr;
    }
    return static_cast<T *>(System->getEntity(m_Handle));
  }
};

static_assert(sizeof(CHandle<int>) == 0x4);
