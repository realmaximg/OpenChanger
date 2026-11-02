#pragma once

#include "sdk/macros.h"

#define Ptm_pNetworkGameClient                                                 \
  "48 8B 89 * * * * 48 85 C9 74 ? 48 8D 15 ? ? ? ? E9 ? ? ? ? C3"

class CNetworkGameClient;

class CNetworkClientService {
public:
  SDK_INTERFACE(Engine2, CNetworkClientService, "NetworkClientService_001");

  PATTERN_FIELD(Engine2, CNetworkGameClient *, m_pNetworkGameClient,
                Ptm_pNetworkGameClient);
};
