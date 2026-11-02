#pragma once

#include "sdk/client/CBasePlayerController.h"
#include "sdk/client/CHandle.h"
#include "sdk/macros.h"

#include <cstdint>

class CCSPlayerController_InventoryServices;
class C_CSPlayerPawn;

class CCSPlayerController : public CBasePlayerController {
public:
  SDK_CLASS(CCSPlayerController);

  SCHEMA_FIELD(CHandle<C_CSPlayerPawn>, m_hPlayerPawn);
  SCHEMA_FIELD(CCSPlayerController_InventoryServices *, m_pInventoryServices);

  static std::uint32_t getLocalIndex();
  static CCSPlayerController *getLocal();
  static C_CSPlayerPawn *getLocalPawn();

  C_CSPlayerPawn *getPawn();
};
