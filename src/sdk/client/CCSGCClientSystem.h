#pragma once

#include "sdk/client/CBaseGameSystemFactory.h"
#include "sdk/client/CGCClient.h"
#include "sdk/macros.h"

#define Ptm_GCClient                                                           \
  "F3 4C 0F 2C C1 48 81 C1 * * * * 45 33 C9 BA E8 03 00 00 4C 03 C0 E9"

class CCSGCClientSystem {
public:
  SDK_GAME_SYSTEM(CCSGCClientSystem, "CCSGCClientSystem");

  PATTERN_FIELD(Client, GCSDK::CGCClient, m_GCClient, Ptm_GCClient);
};
