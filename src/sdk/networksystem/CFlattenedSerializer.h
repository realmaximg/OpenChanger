#pragma once

#include "sdk/macros.h"
#include "sdk/networksystem/CNetworkFieldChangedEventQueue.h"

#include <cstdint>

#define PtDecode "8B 45 6F 89 44 24 28 44 89 6C 24 20 E8 * * * *"

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
