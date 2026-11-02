#pragma once

#include "sdk/client/CEconItemDefinition.h"
#include "sdk/macros.h"

#include <cstdint>

#define PtLoadoutSlot "89 83 * * * * 48 C7 83 6C 03 00 00 FF FF FF FF"
#define PtUsedByClasses "8B 8B * * * * 33 D2 0F AB C1"
#define PtVoPrefix                                                             \
  "48 8B 03 48 8B CB FF 50 48 48 8B C8 48 8D 15 ? ? ? ? FF 15 ? ? ? ? 48 85 "  \
  "C0 74 ? 45 33 C9 45 33 C0 33 D2 48 8B C8 FF 15 ? ? ? ? EB ? 49 8B C6 48 "   \
  "89 83 * * * *"

class CCStrike15ItemDefinition : public CEconItemDefinition {
public:
  SDK_CLASS(CCStrike15ItemDefinition);

  PATTERN_FIELD(Client, std::int32_t, LoadoutSlot, PtLoadoutSlot);
  PATTERN_FIELD(Client, std::uint32_t, UsedByClasses, PtUsedByClasses);
  PATTERN_FIELD(Client, const char *, VoPrefix, PtVoPrefix);

  bool isUsedByClass(std::uint8_t Class) {
    return (UsedByClasses() & (1U << Class)) != 0;
  }
};
