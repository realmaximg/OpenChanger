#pragma once

#include "sdk/client/CAnimGraphControllerBase.h"
#include "sdk/client/CHandle.h"
#include "sdk/client/C_CS2HudModelArms.h"
#include "sdk/client/C_CSPlayerPawnBase.h"
#include "sdk/client/C_EconItemView.h"
#include "sdk/macros.h"

#include <cstdint>
#include <vector>

class C_CSWeaponBase;

class C_CSPlayerPawn : public C_CSPlayerPawnBase {
public:
  SDK_CLASS(C_CSPlayerPawn);

  SCHEMA_FIELD(C_EconItemView, m_EconGloves);
  SCHEMA_FIELD(CHandle<C_CS2HudModelArms>, m_hHudModelArms);

  std::vector<C_CSWeaponBase *> getWeapons();

  CAnimGraphControllerBase *getHudArmsGraph() {
    C_CS2HudModelArms *const Arms = m_hHudModelArms().get();
    return Arms == nullptr ? nullptr : Arms->m_pMainGraphController();
  }
};
