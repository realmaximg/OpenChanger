#include "sdk/client/CCSPlayerController.h"

#include "sdk/client/CGameEntitySystem.h"
#include "sdk/client/C_CSPlayerPawn.h"
#include "sdk/engine2/CEngineClient.h"
#include "sdk/engine2/CPlayerSlot.h"

#include <cstdint>

std::uint32_t CCSPlayerController::getLocalIndex() {
  CPlayerSlot Slot{-1};
  CEngineClient::get()->GetLocalPlayer(&Slot, 0);
  return Slot.m_Data < 0 ? 0 : static_cast<std::uint32_t>(Slot.m_Data) + 1;
}

CCSPlayerController *CCSPlayerController::getLocal() {
  const std::uint32_t Index = getLocalIndex();
  CGameEntitySystem *const System = CGameEntitySystem::get();
  if (Index == 0 || System == nullptr) {
    return nullptr;
  }
  return static_cast<CCSPlayerController *>(System->getEntityByIndex(Index));
}

C_CSPlayerPawn *CCSPlayerController::getLocalPawn() {
  CCSPlayerController *const My = getLocal();
  return My == nullptr ? nullptr : My->getPawn();
}

C_CSPlayerPawn *CCSPlayerController::getPawn() { return m_hPlayerPawn().get(); }
