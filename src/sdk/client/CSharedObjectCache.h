#pragma once

#include "sdk/client/CSharedObject.h"
#include "sdk/client/CSharedObjectTypeCache.h"
#include "sdk/macros.h"
#include "sdk/tier1/CUtlVector.h"

#include <cstdint>
#include <span>

#define Ptm_CacheObjects "89 4C 24 34 8B 56 * 8B D9 85 D2 89 4D 87"

namespace GCSDK {

// NOLINTNEXTLINE(performance-enum-size)
enum ESOCacheEvent : std::int32_t {
  eSOCacheEvent_Incremental = 4,
};

class CSharedObjectCache {
public:
  SDK_CLASS(CSharedObjectCache);

  PATTERN_FIELD(Client, CUtlVector<CSharedObjectTypeCache *>, m_CacheObjects,
                Ptm_CacheObjects);

  std::span<CSharedObject *> objectsOf(std::int32_t Type) {
    for (CSharedObjectTypeCache *const Types : m_CacheObjects().span()) {
      if (Types != nullptr && Types->m_nTypeID() == Type) {
        return Types->m_vecObjects().span();
      }
    }
    return {};
  }
};

} // namespace GCSDK
