#pragma once

#include "sdk/macros.h"
#include "sdk/networksystem/CNetworkFieldChangedEventQueue.h"

#include <cstdint>

#define PtDecode                                                               \
  "4C 89 44 24 18 55 53 41 55 48 8D 6C 24 ? 48 81 EC ? ? ? ? 4C 8B 6A 08 48 "  \
  "8B D9 4D 85 ED 75 ? 32 C0 48 81 C4 ? ? ? ? 41 5D 5B 5D C3 8B 45 ? 8B 4D ? "  \
  "48 89 B4 24 ? ? ? ? 48 89 BC 24 ? ? ? ? 33 FF 89 45 ? 48 8D 45 ? 4C 89 A4 "  \
  "24 ? ? ? ? 4C 89 4D ? 89 4D ? C6 45 ? 00 C7 45 ? 08 00 00 00 48 89 7D ? C7 "  \
  "45 ? 08 00 00 00"

class CFlattenedSerializer {
public:
  SDK_CLASS(CFlattenedSerializer);

  PATTERN_FUNCTION(NetworkSystem,
                   std::uint8_t(CFlattenedSerializer *, void *, void *, void *,
                                std::int32_t, std::int32_t,
                                CNetworkFieldChangedEventQueue *,
                                std::uint32_t),
                   Decode, PtDecode);
};
