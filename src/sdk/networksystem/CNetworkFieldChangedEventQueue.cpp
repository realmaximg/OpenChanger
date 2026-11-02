#include "sdk/networksystem/CNetworkFieldChangedEventQueue.h"

#include "game/link.h"
#include "sdk/client/CEntityInstance.h"

#include <cstdint>

namespace {

constexpr std::int32_t NoChange = -1;

} // namespace

CNetworkFieldChangedEventQueue::Pending
CNetworkFieldChangedEventQueue::findChange(CEntityInstance &Entity,
                                           const oc::AddressLink &Callback,
                                           std::int32_t Element) {
  const void *const Function = Callback.address();
  Pending Found;
  for (std::int32_t Thread = 0; Thread < m_nThreadQueues(); ++Thread) {
    ThreadQueue_t &Queue = threadQueue(Thread);
    Queue.m_Mutex.lock();
    for (std::int32_t Priority = 0; Priority < m_nPriorities(); ++Priority) {
      Changes_t &Changes = Queue.priority(Priority);
      for (std::int32_t Index = Changes.m_nHead(); Index != NoChange;) {
        Change_t &Change = Changes.at(Index);
        if (!Change.m_bCancelled() && Change.m_pEntity() == &Entity &&
            Change.m_pFunction() == Function &&
            Change.m_nElement() == Element) {
          Found = {.Object = Change.m_pObject(), .Value = Change.m_pValue()};
        }
        Index = Change.m_nNext();
      }
    }
    Queue.m_Mutex.unlock();
  }
  return Found;
}
