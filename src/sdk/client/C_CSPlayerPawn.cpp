#include "sdk/client/C_CSPlayerPawn.h"

#include "sdk/client/CPlayer_WeaponServices.h"
#include "sdk/client/C_BasePlayerWeapon.h"
#include "sdk/client/C_CSWeaponBase.h"

#include <vector>

std::vector<C_CSWeaponBase *> C_CSPlayerPawn::getWeapons() {
  std::vector<C_CSWeaponBase *> Weapons;
  CPlayer_WeaponServices *const Services = m_pWeaponServices();
  if (Services == nullptr) {
    return Weapons;
  }
  for (const auto &Handle : Services->m_hMyWeapons().span()) {
    if (C_BasePlayerWeapon *const Weapon = Handle.get()) {
      Weapons.push_back(static_cast<C_CSWeaponBase *>(Weapon));
    }
  }
  return Weapons;
}
