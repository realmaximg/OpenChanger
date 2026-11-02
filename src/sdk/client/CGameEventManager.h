#pragma once

#include "sdk/client/IGameEvent.h"
#include "sdk/macros.h"

#define PtFireEventClientSide                                                  \
  "45 33 C0 48 89 44 24 ? 48 8B CF FF D3 48 8B 0D ? ? ? ? 48 8B D7 48 8B 01 "  \
  "FF 50 *"
#define PtGameEventManager                                                     \
  "45 33 C0 48 89 44 24 ? 48 8B CF FF D3 48 8B 0D * * * * 48 8B D7 48 8B 01 "  \
  "FF 50 ?"

class CGameEventManager {
public:
  SDK_CLASS(CGameEventManager);

  PATTERN_GLOBAL(Client, CGameEventManager *, GameEventManager,
                 PtGameEventManager);
  PATTERN_VIRTUAL(Client, bool(IGameEvent *), FireEventClientSide,
                  PtFireEventClientSide);

  static CGameEventManager *get() { return GameEventManager(); }
};
