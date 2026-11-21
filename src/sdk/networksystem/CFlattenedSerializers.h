#pragma once

#include "sdk/macros.h"
#include "sdk/networksystem/CNetworkFieldChangedEventQueue.h"

#include <cstdint>

#define PtDecode                                                               \
  "4C 89 44 24 18 55 53 41 55 48 8D 6C 24 ? 48 81 EC ? ? ? ? 4C 8B 6A 08 48 "  \
  "8B D9 4D 85 ED 75 ? 32 C0 48 81 C4 ? ? ? ? 41 5D 5B 5D C3 8B 45 ? 8B 4D ? " \
  "48 89 B4 24 ? ? ? ? 48 89 BC 24 ? ? ? ? 33 FF 89 45 ? 48 8D 45 ? 4C 89 A4 " \
  "24 ? ? ? ? 4C 89 4D ? 89 4D ? C6 45 ? 00 C7 45 ? 08 00 00 00 48 89 7D ? "   \
  "C7 45 ? 08 00 00 00"
#define PtDecodeDelta                                                          \
  "4C 89 44 24 18 48 89 54 24 10 55 57 41 54 41 56 41 57 48 8D 6C 24 ? 48 "    \
  "81 EC ? ? ? ? 4C 8B 65 ? 45 33 FF 4D 8B F1 48 8B F9 4D 85 E4 74 ? 45 89 "   \
  "3C 24 48 8B 42 08"

class CFlattenedSerializers {
public:
  SDK_CLASS(CFlattenedSerializers);

  PATTERN_FUNCTION(NetworkSystem,
                   std::uint8_t(CFlattenedSerializers *, void *, void *, void *,
                                std::int32_t, std::int32_t,
                                CNetworkFieldChangedEventQueue *,
                                std::uint32_t),
                   Decode, PtDecode);
  PATTERN_FUNCTION(NetworkSystem,
                   std::uint8_t(CFlattenedSerializers *, void *, void *, void *,
                                std::int32_t, std::int32_t,
                                CNetworkFieldChangedEventQueue *,
                                std::int32_t *),
                   DecodeDelta, PtDecodeDelta);
};
