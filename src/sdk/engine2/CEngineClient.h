#pragma once

#include "sdk/engine2/CPlayerSlot.h"
#include "sdk/macros.h"

#include <cstdint>

#define PtGetLocalPlayer                                                       \
  "48 8B 0D ? ? ? ? 48 8D 54 24 ? 45 33 C0 BB FF FF FF FF 48 8B 01 FF 90 * * " \
  "* *"

class CEngineClient {
public:
  SDK_INTERFACE(Engine2, CEngineClient, "Source2EngineToClient001");

  PATTERN_VIRTUAL(Client, void(CPlayerSlot *, std::int32_t), GetLocalPlayer,
                  PtGetLocalPlayer);
};
