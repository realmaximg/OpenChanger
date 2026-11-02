#pragma once

#include "sdk/client/CAnimGraphControllerBase.h"
#include "sdk/macros.h"

#include <cstdint>

#define PtCachedState "F3 41 0F 10 BF ? ? ? ? 66 3B B7 * * * *"
#define PtInspectComplete "48 8B 44 0F ? 48 3B 05 ? ? ? ? 75 ? C6 83 * * * * 01"
#define PtInspectPhase "0F 84 ? ? ? ? C7 83 * * * * 03 00 00 00"
#define PtReflectWeaponState                                                   \
  "40 55 53 57 41 56 41 57 48 8D 6C 24 C9 48 81 EC A0 00 00 00 48 8D 05"

class C_CSWeaponBase;

class CCS2WeaponGraphController : public CAnimGraphControllerBase {
public:
  SDK_CLASS(CCS2WeaponGraphController);

  static constexpr std::int32_t InspectOutroPhase = 3;

  PATTERN_FIELD(Client, std::uint16_t, CachedState, PtCachedState);
  PATTERN_FIELD(Client, bool, InspectComplete, PtInspectComplete);
  PATTERN_FIELD(Client, std::int32_t, InspectPhase, PtInspectPhase);
  PATTERN_FUNCTION(Client, void(CCS2WeaponGraphController *, C_CSWeaponBase *),
                   ReflectWeaponState, PtReflectWeaponState);
};
