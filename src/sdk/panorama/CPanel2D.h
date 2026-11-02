#pragma once

#include "sdk/macros.h"

#define PtUIPanel                                                              \
  "48 83 EC 28 48 8B 05 ? ? ? ? 48 8D 15 ? ? ? ? 48 8B 48 * 48 8B 01 FF 90 "   \
  "78 01 00 00"

class CUIPanel;

class CPanel2D {
public:
  SDK_CLASS(CPanel2D);

  PATTERN_FIELD(Client, CUIPanel *, UIPanel, PtUIPanel);
};
