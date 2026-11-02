#pragma once

#include "sdk/client/CSharedObject.h"
#include "sdk/macros.h"
#include "sdk/tier1/CUtlVector.h"

#include <cstdint>

#define Ptm_vecObjects "48 8B F8 48 85 C0 74 42 41 8B DF 44 39 78 * 76 39"
#define Ptm_nTypeID "4A 8B 0C F0 48 85 C9 0F 84 ? ? ? ? 8B 41 * 89 44 24 38"

namespace GCSDK {

class CSharedObjectTypeCache {
public:
  SDK_CLASS(CSharedObjectTypeCache);

  PATTERN_FIELD(Client, CUtlVector<CSharedObject *>, m_vecObjects,
                Ptm_vecObjects);
  PATTERN_FIELD(Client, std::int32_t, m_nTypeID, Ptm_nTypeID);
};

} // namespace GCSDK
