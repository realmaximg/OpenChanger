#pragma once

#include "sdk/macros.h"

#include <cstddef>
#include <cstdint>

struct CUtlStringToken {
  SDK_CLASS(CUtlStringToken);

  SDK_EXPORT_FUNCTION(Tier0, CUtlStringToken *(CUtlStringToken *, const char *),
                      Make, "?Make@CUtlStringToken@@SA?AV1@PEBD@Z");

  std::uint32_t m_nHashCode = 0;
};

static_assert(sizeof(CUtlStringToken) == 0x4);
static_assert(offsetof(CUtlStringToken, m_nHashCode) == 0x0);
