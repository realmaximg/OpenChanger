#pragma once

#include "sdk/macros.h"

#include <cstddef>
#include <cstdint>

class CAtomicMutex {
public:
  SDK_CLASS(CAtomicMutex);

  SDK_EXPORT_FUNCTION(Tier0, void(CAtomicMutex *, std::uint32_t), AcquireLock,
                      "?AcquireLock@CAtomicMutex@@AEAAXI@Z");
  SDK_EXPORT_FUNCTION(Tier0, void(const std::uint32_t *), ThreadAtomicNotifyOne,
                      "?ThreadAtomicNotifyOne@@YAXPEBI@Z");

  void lock();
  void unlock();

  std::uint32_t m_nState;
  std::uint16_t m_nDepth;
  PAD(0x2);
  std::uint32_t m_nOwner;
};

static_assert(offsetof(CAtomicMutex, m_nState) == 0x0);
static_assert(offsetof(CAtomicMutex, m_nDepth) == 0x4);
static_assert(offsetof(CAtomicMutex, m_nOwner) == 0x8);
