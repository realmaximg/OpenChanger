#pragma once

#include "sdk/macros.h"

#define PtForceFullUpdate                                                      \
  "48 8B 89 ? ? ? ? 48 85 C9 74 ? 48 8D 15 ? ? ? ? E9 * * * * C3"

class CNetworkGameClient {
public:
  SDK_CLASS(CNetworkGameClient);

  PATTERN_FUNCTION(Engine2, void(CNetworkGameClient *, const char *),
                   ForceFullUpdate, PtForceFullUpdate);
};
