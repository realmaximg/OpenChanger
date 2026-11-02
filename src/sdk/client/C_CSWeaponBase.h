#pragma once

#include "sdk/client/C_BasePlayerWeapon.h"
#include "sdk/macros.h"

#include <cstdint>

class C_CSWeaponBase : public C_BasePlayerWeapon {
public:
  SDK_CLASS(C_CSWeaponBase);

  static constexpr std::uint16_t IdleState = 50;
  static constexpr std::uint16_t InspectState = 1000;

  SCHEMA_FIELD(std::uint16_t, m_iWeaponGameplayAnimState);
  SCHEMA_FIELD(float, m_flInspectCancelCompleteTime);
  SCHEMA_FIELD(bool, m_bInspectShouldLoop);
};
