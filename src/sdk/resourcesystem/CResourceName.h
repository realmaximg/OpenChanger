#pragma once

#include "sdk/macros.h"
#include "sdk/tier1/CBufferString.h"

#include <array>
#include <cstddef>
#include <cstdint>

#define PtSet                                                                  \
  "48 89 5C 24 10 57 48 83 EC 30 8B 41 04 48 8D 79 08 48 8B D9 A9 FF FF FF "   \
  "3F 76 0F 0F BA E0 1E 48 8B C7 72 03 48 8B 07 C6 00 00 81 21 00 00 00 C0"

class CResourceName {
public:
  SDK_CLASS(CResourceName);

  PATTERN_FUNCTION(Client, bool(CResourceName *, const char *), Set, PtSet);

  explicit CResourceName(const char *Path) { Set(this, Path); }

  CBufferStringN<0xd0> m_Name;
  std::uint64_t m_nResourceId = 0;
  PAD(0x8);
};

static_assert(sizeof(CResourceName) == 0xe0);
static_assert(offsetof(CResourceName, m_nResourceId) == 0xd0);
