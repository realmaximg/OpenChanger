#include "features/changer/Changer.h"
#include "features/changer/catalog/catalog.h"
#include "features/changer/loadout/loadout.h"
#include "game/hook.h"
#include "sdk/client/CCS2WeaponGraphController.h"
#include "sdk/client/CCSPlayerController.h"
#include "sdk/client/C_CSPlayerPawn.h"
#include "sdk/client/C_CSWeaponBase.h"

namespace oc::changer {

namespace {

void detour(CCS2WeaponGraphController *Controller, C_CSWeaponBase *Weapon);

InlineHook<detour> Hook{CCS2WeaponGraphController::ReflectWeaponStateLink};

bool holdsIdle(CCS2WeaponGraphController &Controller, C_CSWeaponBase &Weapon) {
  return Weapon.m_iWeaponGameplayAnimState() == C_CSWeaponBase::IdleState &&
         Weapon.m_flInspectCancelCompleteTime() == 0.0F &&
         !Weapon.m_bInspectShouldLoop() && Controller.InspectPhase() != 0 &&
         !Controller.InspectComplete();
}

bool restarts(CCS2WeaponGraphController &Controller, C_CSWeaponBase &Weapon) {
  return Weapon.m_iWeaponGameplayAnimState() == C_CSWeaponBase::InspectState &&
         Weapon.m_bInspectShouldLoop() &&
         Controller.InspectPhase() ==
             CCS2WeaponGraphController::InspectOutroPhase;
}

bool ourHudKnife(CCS2WeaponGraphController &Controller,
                 C_CSWeaponBase &Weapon) {
  C_CSPlayerPawn *const Pawn = CCSPlayerController::getLocalPawn();
  if (Pawn == nullptr || Pawn->getHudArmsGraph() != &Controller) {
    return false;
  }
  const auto Items = Changer::get().loadout();
  return Items->knifeById(Weapon.getItem().getItemId(),
                          Changer::get().catalog().Ids) != nullptr;
}

void detour(CCS2WeaponGraphController *Controller, C_CSWeaponBase *Weapon) {
  if (Weapon != nullptr && holdsIdle(*Controller, *Weapon) &&
      ourHudKnife(*Controller, *Weapon)) {
    return;
  }
  if (Weapon != nullptr && restarts(*Controller, *Weapon) &&
      ourHudKnife(*Controller, *Weapon)) {
    Controller->CachedState() = 0;
  }
  Hook.original(Controller, Weapon);
}

} // namespace

} // namespace oc::changer
