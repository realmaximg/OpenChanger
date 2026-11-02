#include "sdk/tier0/CAtomicMutex.h"

#include <windows.h>

#include <atomic>
#include <cstdint>

namespace {

constexpr std::uint32_t Locked = 3;

} // namespace

void CAtomicMutex::lock() {
  const std::uint32_t Thread = GetCurrentThreadId();
  if (m_nOwner == Thread) {
    ++m_nDepth;
    return;
  }
  std::uint32_t Free = 0;
  if (!std::atomic_ref(m_nState).compare_exchange_strong(Free, Locked)) {
    AcquireLock(this, Thread);
    return;
  }
  m_nDepth = 1;
  m_nOwner = Thread;
}

void CAtomicMutex::unlock() {
  if (--m_nDepth != 0) {
    return;
  }
  m_nOwner = 0;
  if (std::atomic_ref(m_nState).fetch_sub(Locked) != Locked) {
    ThreadAtomicNotifyOne(&m_nState);
  }
}
