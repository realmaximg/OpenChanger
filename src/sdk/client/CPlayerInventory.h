#pragma once

#include "sdk/client/CGCClientSharedObjectCache.h"
#include "sdk/client/CSharedObject.h"
#include "sdk/client/CSharedObjectCache.h"
#include "sdk/client/SOID_t.h"
#include "sdk/macros.h"

#define Ptm_OwnerID                                                            \
  "8B 41 18 49 8B F0 4C 8B F1 39 42 08 0F 85 ? ? ? ? 48 8B 41 * 48 39 02 0F "  \
  "85"
#define Ptm_pSOCache                                                           \
  "49 89 76 * 48 85 F6 0F 84 ? ? ? ? 41 8B 56 20 48 8D 4D 0F 41 B8 01 00 00 "  \
  "00"
#define PtSOCreated                                                            \
  "48 89 5C 24 08 57 48 83 EC 30 8B 41 18 49 8B F8 48 8B D9 39 42 08 75 ? 48 " \
  "8B 41 10 48 39 02 75 ? 48 83 79 68 00 74 ? 41 83 F9 04 75"
#define PtSOUpdated                                                            \
  "48 89 5C 24 18 57 48 83 EC 30 8B 41 18 49 8B F8 48 8B D9 39 42 08 0F 85"

class CPlayerInventory {
public:
  SDK_CLASS(CPlayerInventory);

  PATTERN_FIELD(Client, GCSDK::SOID_t, m_OwnerID, Ptm_OwnerID);
  PATTERN_FIELD(Client, GCSDK::CGCClientSharedObjectCache *, m_pSOCache,
                Ptm_pSOCache);
  PATTERN_FUNCTION(Client,
                   void(CPlayerInventory *, const GCSDK::SOID_t *,
                        GCSDK::CSharedObject *, GCSDK::ESOCacheEvent),
                   SOCreated, PtSOCreated);
  PATTERN_FUNCTION(Client,
                   void(CPlayerInventory *, const GCSDK::SOID_t *,
                        GCSDK::CSharedObject *, GCSDK::ESOCacheEvent),
                   SOUpdated, PtSOUpdated);
};
