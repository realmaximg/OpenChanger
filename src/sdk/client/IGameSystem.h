#pragma once

#include "sdk/macros.h"

#define PtLoopDeactivateAllSystems                                             \
  "48 83 EC 58 48 89 6C 24 68 48 8B E9 8B 0D ? ? ? ? 48 89 74 24 70 0F B6 F2 " \
  "BA 02 00 00 00"

class IGameSystem {
public:
  SDK_CLASS(IGameSystem);

  PATTERN_FUNCTION(Client, void(void *, bool), LoopDeactivateAllSystems,
                   PtLoopDeactivateAllSystems);
};
