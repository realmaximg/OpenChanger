#include "sdk/client/C_BasePlayerPawn.h"

#include "sdk/client/CCSPlayerController.h"
#include "sdk/client/CGameEntitySystem.h"

#include <cstdint>

bool C_BasePlayerPawn::hasLocalController() {
  const std::uint32_t Index = CCSPlayerController::getLocalIndex();
  return Index != 0 &&
         (m_hController().m_Handle & CGameEntitySystem::IndexMask) == Index;
}
