#pragma once

#include "sdk/macros.h"

#include <cstdint>

#define PtKeychainName                                                         \
  "48 8B 5E 20 48 3B C3 74 ? 48 85 C0 74 ? 40 38 28 74 ? 48 8B C8 FF 15 ? ? "  \
  "? ? EB ? 48 8B C5 48 89 46 *"

class CKeychainDefinition {
public:
  SDK_CLASS(CKeychainDefinition);

  PATTERN_FIELD(Client, const char *, KeychainName, PtKeychainName);
};
