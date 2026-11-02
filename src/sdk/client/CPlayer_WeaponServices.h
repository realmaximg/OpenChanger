#pragma once

#include "sdk/client/CHandle.h"
#include "sdk/client/CPlayerPawnComponent.h"
#include "sdk/macros.h"
#include "sdk/tier1/CUtlVector.h"

class C_BasePlayerWeapon;

class CPlayer_WeaponServices : public CPlayerPawnComponent {
public:
  SDK_CLASS(CPlayer_WeaponServices);

  SCHEMA_FIELD(CUtlVector<CHandle<C_BasePlayerWeapon>>, m_hMyWeapons);
};
