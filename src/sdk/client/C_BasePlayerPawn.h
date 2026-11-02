#pragma once

#include "sdk/client/CHandle.h"
#include "sdk/client/CPlayer_WeaponServices.h"
#include "sdk/client/C_BaseCombatCharacter.h"
#include "sdk/macros.h"

class CCSPlayerController;

class C_BasePlayerPawn : public C_BaseCombatCharacter {
public:
  SDK_CLASS(C_BasePlayerPawn);

  SCHEMA_FIELD(CPlayer_WeaponServices *, m_pWeaponServices);
  SCHEMA_FIELD(CHandle<CCSPlayerController>, m_hController);

  bool hasLocalController();
};
