#pragma once

#include <atomic>
#include <cstddef>
#include <cstdint>

class CRefCounted {
public:
  struct VTable {
    void (*Destructor)(CRefCounted *, bool);
  };

  const VTable *m_pVTable;
  std::int32_t m_iRefs;

  void Release() {
    if (std::atomic_ref(m_iRefs).fetch_sub(1) == 1) {
      m_pVTable->Destructor(this, true);
    }
  }
};

static_assert(offsetof(CRefCounted, m_pVTable) == 0x0);
static_assert(offsetof(CRefCounted, m_iRefs) == 0x8);
