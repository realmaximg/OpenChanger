#pragma once

#include "sdk/resourcesystem/ResourceBinding_t.h"

template <class T> struct CStrongHandle {
  ResourceBinding_t *m_pBinding;
};

static_assert(sizeof(CStrongHandle<int>) == 0x8);
