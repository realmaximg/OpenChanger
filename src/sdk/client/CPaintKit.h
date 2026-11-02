#pragma once

#include "sdk/macros.h"
#include "sdk/tier1/CUtlString.h"

#include <cstdint>

#define PtPaintKitName                                                         \
  "48 8D 15 ? ? ? ? 4C 8B FF 48 8B CE 4C 0F 45 F8 45 0F B6 E9 4D 8B F0 FF 15 " \
  "? ? ? ? 48 85 C0 74 ? 45 33 C9 45 33 C0 49 8B D7 48 8B C8 FF 15 ? ? ? ? "   \
  "4C 8B F8 49 8B D7 48 8D 4B *"
#define PtPaintKitRarity "0F B6 84 24 70 01 00 00 48 8B CF 41 89 45 *"
#define PtWearRemapMin                                                         \
  "48 8D 15 ? ? ? ? F3 41 0F 10 56 6C 48 8B CE FF 15 ? ? ? ? F3 0F 11 43 *"
#define PtWearRemapMax                                                         \
  "48 8D 15 ? ? ? ? F3 41 0F 10 56 70 48 8B CE FF 15 ? ? ? ? F3 0F 11 43 *"
#define PtUseLegacyModel                                                       \
  "48 8D 15 ? ? ? ? 48 8B CE 85 C0 0F 95 C0 45 33 C0 88 83 AD 00 00 00 FF 15 " \
  "? ? ? ? 4C 8B FF 48 8D 15 ? ? ? ? 85 C0 48 8B CE 0F 95 C0 88 83 * * * *"

class CPaintKit {
public:
  SDK_CLASS(CPaintKit);

  PATTERN_FIELD(Client, CUtlString, PaintKitName, PtPaintKitName);
  PATTERN_FIELD(Client, std::int32_t, PaintKitRarity, PtPaintKitRarity);
  PATTERN_FIELD(Client, float, WearRemapMin, PtWearRemapMin);
  PATTERN_FIELD(Client, float, WearRemapMax, PtWearRemapMax);
  PATTERN_FIELD(Client, bool, UseLegacyModel, PtUseLegacyModel);
};
