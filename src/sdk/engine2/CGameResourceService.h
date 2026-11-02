#pragma once

#include "sdk/macros.h"

#define PtEntitySystem                                                         \
  "48 81 EC F0 00 00 00 48 83 79 * 00 49 8B F1 41 8B F8 8B EA 48 8B D9 74 ? "  \
  "45 85 C0 74 ?"

class CGameEntitySystem;

class CGameResourceService {
public:
  SDK_INTERFACE(Engine2, CGameResourceService, "GameResourceServiceClientV001");

  PATTERN_FIELD(Engine2, CGameEntitySystem *, EntitySystem, PtEntitySystem);
};
