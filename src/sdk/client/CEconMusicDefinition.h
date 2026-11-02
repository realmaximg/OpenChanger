#pragma once

#include "sdk/macros.h"

#define PtMusicName "E8 ? ? ? ? 48 85 C0 74 ? 48 8B 40 * 4C 8B C7"

class CEconMusicDefinition {
public:
  SDK_CLASS(CEconMusicDefinition);

  PATTERN_FIELD(Client, const char *, MusicName, PtMusicName);
};
