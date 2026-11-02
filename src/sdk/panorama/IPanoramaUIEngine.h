#pragma once

#include "sdk/macros.h"

#define PtAccessUIEngine                                                       \
  "48 8B 01 FF 50 * 48 85 C0 75 ? 48 83 C4 20 5B C3 48 8B C8 E8"

class CUIEngine;

class IPanoramaUIEngine {
public:
  SDK_INTERFACE(Panorama, IPanoramaUIEngine, "PanoramaUIEngine001");

  PATTERN_VIRTUAL(Client, CUIEngine *(), AccessUIEngine, PtAccessUIEngine);
};
