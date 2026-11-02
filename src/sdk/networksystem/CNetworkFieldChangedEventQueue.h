#pragma once

#include "game/link.h"
#include "sdk/client/CEntityInstance.h"
#include "sdk/macros.h"
#include "sdk/tier0/CAtomicMutex.h"

#include <cstdint>

#define Ptm_pThreadQueues                                                      \
  "48 8D 14 85 00 00 00 00 49 C1 E6 05 48 89 54 24 58 ? ? ? ? ? ? ? ? ? ? ? "  \
  "? 48 8B 46 28 39 1C 02 0F 8C ? ? ? ? 89 8E D8 00 00 00 4C 8B 6E * 48 63 "   \
  "46 08 4C 89 6C 24 50 48 8D 1C 40 48 C1 E3 05 49 03 DD 48 89 5C 24 60 4C "   \
  "3B EB 0F 84 ? ? ? ? ? ? ? ? ? ? ? FF 15 ? ? ? ? 8B D0 41 39 45 08 75 07 "   \
  "66 41 FF 45 04 EB ? 41 8B 45 00 ? 85 C0 75 ? F0 45 0F B1 65 00 75 ? 66 "    \
  "45 89 7D 04 41 89 55 08"
#define Ptm_nThreadQueues "4C 8B 6E 10 48 63 46 * 4C 89 6C 24 50"
#define Ptm_nPriorities "FF C1 89 4C 24 40 3B 8E * * * * 0F 8D"
#define PtThreadQueue_t_m_pPriorities "4D 8B 65 * 43 8B 44 26 10 83 F8 FF 0F 84"
#define PtChanges_t_m_nHead "4D 8B 65 18 43 8B 44 26 * 83 F8 FF 0F 84"
#define PtChanges_t_m_pMemory "44 8B AC 24 ? ? ? ? 4D 8D 7C 24 * 4D 03 FE"
#define PtChange_t_m_bCancelled "49 8B 1F 48 8D 3C C0 48 C1 E7 04 80 7C 1F * 00"
#define PtChange_t_m_pFunction                                                 \
  "48 8B 44 1F * 4C 8B 4C 1F 20 44 8B 44 1F 60 48 8B 54 1F 18 48 8B 4C 1F 08 " \
  "FF D0"
#define PtChange_t_m_pValue                                                    \
  "48 8B 44 1F 10 4C 8B 4C 1F * 44 8B 44 1F 60 48 8B 54 1F 18 48 8B 4C 1F 08 " \
  "FF D0"
#define PtChange_t_m_nElement                                                  \
  "48 8B 44 1F 10 4C 8B 4C 1F 20 44 8B 44 1F * 48 8B 54 1F 18 48 8B 4C 1F 08 " \
  "FF D0"
#define PtChange_t_m_pEntity                                                   \
  "48 8B 44 1F 10 4C 8B 4C 1F 20 44 8B 44 1F 60 48 8B 54 1F * 48 8B 4C 1F 08 " \
  "FF D0"
#define PtChange_t_m_pObject                                                   \
  "48 8B 44 1F 10 4C 8B 4C 1F 20 44 8B 44 1F 60 48 8B 54 1F 18 48 8B 4C 1F * " \
  "FF D0"
#define PtChange_t_m_nNext "49 8B 07 8B 84 07 * * * * 83 F8 FF 0F 85"

class CNetworkFieldChangedEventQueue {
public:
  SDK_CLASS(CNetworkFieldChangedEventQueue);

  static constexpr std::int32_t NoElement = -1;

  struct Change_t {
    SDK_CLASS(Change_t);

    static constexpr std::uint32_t Size = 0x90;

    PATTERN_FIELD(NetworkSystem, bool, m_bCancelled, PtChange_t_m_bCancelled);
    PATTERN_FIELD(NetworkSystem, void *, m_pFunction, PtChange_t_m_pFunction);
    PATTERN_FIELD(NetworkSystem, void *, m_pValue, PtChange_t_m_pValue);
    PATTERN_FIELD(NetworkSystem, std::int32_t, m_nElement,
                  PtChange_t_m_nElement);
    PATTERN_FIELD(NetworkSystem, CEntityInstance *, m_pEntity,
                  PtChange_t_m_pEntity);
    PATTERN_FIELD(NetworkSystem, void *, m_pObject, PtChange_t_m_pObject);
    PATTERN_FIELD(NetworkSystem, std::int32_t, m_nNext, PtChange_t_m_nNext);
  };

  struct Changes_t {
    SDK_CLASS(Changes_t);

    static constexpr std::uint32_t Size = 0x20;

    PATTERN_FIELD(NetworkSystem, std::int32_t, m_nHead, PtChanges_t_m_nHead);
    PATTERN_FIELD(NetworkSystem, void *, m_pMemory, PtChanges_t_m_pMemory);

    Change_t &at(std::int32_t Index) {
      return oc::elementAt<Change_t>(m_pMemory(), Index, Change_t::Size);
    }
  };

  struct ThreadQueue_t {
    SDK_CLASS(ThreadQueue_t);

    static constexpr std::uint32_t Size = 0x60;

    CAtomicMutex m_Mutex;

    PATTERN_FIELD(NetworkSystem, void *, m_pPriorities,
                  PtThreadQueue_t_m_pPriorities);

    Changes_t &priority(std::int32_t Priority) {
      return oc::elementAt<Changes_t>(m_pPriorities(), Priority,
                                      Changes_t::Size);
    }
  };

  struct Pending {
    void *Object = nullptr;
    void *Value = nullptr;
  };

  PATTERN_FIELD(NetworkSystem, void *, m_pThreadQueues, Ptm_pThreadQueues);
  PATTERN_FIELD(NetworkSystem, std::int32_t, m_nThreadQueues,
                Ptm_nThreadQueues);
  PATTERN_FIELD(NetworkSystem, std::int32_t, m_nPriorities, Ptm_nPriorities);

  ThreadQueue_t &threadQueue(std::int32_t Index) {
    return oc::elementAt<ThreadQueue_t>(m_pThreadQueues(), Index,
                                        ThreadQueue_t::Size);
  }

  Pending findChange(CEntityInstance &Entity, const oc::AddressLink &Callback,
                     std::int32_t Element);
};
