#pragma once

#include "sdk/macros.h"

#define PtFree                                                                 \
  "4C 8B 65 8F 4D 85 E4 74 14 48 8B 0D ? ? ? ? 49 8B D4 48 8B 09 4C 8B 01 41 " \
  "FF 50 * 0F B6 44 24 30"

class IMemAlloc {
public:
  SDK_EXPORT(Tier0, IMemAlloc, "g_pMemAlloc");

  PATTERN_VIRTUAL(Client, void(void *), Free, PtFree);
};
