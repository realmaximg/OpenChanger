#pragma once

#include "sdk/macros.h"

#include <cstdint>

#define PtStickerSchema "8B 05 * * * * 48 8B 4C 24 40 83 49 10 04 89 41 28"
#define PtStickerOffsetX                                                       \
  "F3 0F 10 05 * * * * 81 48 10 80 00 00 00 F3 0F 11 40 3C"
#define PtStickerOffsetY                                                       \
  "F3 0F 10 0D * * * * 81 48 10 00 01 00 00 F3 0F 11 48 40 0F 57 C9"
#define PtStickerRotation                                                      \
  "F3 0F 10 05 * * * * 48 8B 44 24 40 83 48 10 20 F3 0F 11 40 34"

class CUI_Item3dPanel {
public:
  SDK_CLASS(CUI_Item3dPanel);

  PATTERN_GLOBAL(Client, std::int32_t, StickerSchema, PtStickerSchema);
  PATTERN_GLOBAL(Client, float, StickerOffsetX, PtStickerOffsetX);
  PATTERN_GLOBAL(Client, float, StickerOffsetY, PtStickerOffsetY);
  PATTERN_GLOBAL(Client, float, StickerRotation, PtStickerRotation);
};
