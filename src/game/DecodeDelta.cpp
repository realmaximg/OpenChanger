#include "game/DecodedHandler.h"
#include "game/hook.h"
#include "sdk/networksystem/CFlattenedSerializers.h"
#include "sdk/networksystem/CNetworkFieldChangedEventQueue.h"

#include <cstdint>

namespace oc {

namespace {

// NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
std::uint8_t detour(CFlattenedSerializers *Serializers, void *A2, void *A3,
                    void *A4, std::int32_t Entity, std::int32_t A6,
                    CNetworkFieldChangedEventQueue *Changes,
                    std::int32_t *FieldCount);

InlineHook<detour> Hook{CFlattenedSerializers::DecodeDeltaLink};

// NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
std::uint8_t detour(CFlattenedSerializers *Serializers, void *A2, void *A3,
                    void *A4, std::int32_t Entity, std::int32_t A6,
                    CNetworkFieldChangedEventQueue *Changes,
                    std::int32_t *FieldCount) {
  const std::uint8_t Result =
      Hook.original(Serializers, A2, A3, A4, Entity, A6, Changes, FieldCount);
  if (Result != 0) {
    dispatchDecoded(Entity, Changes);
  }
  return Result;
}

} // namespace

} // namespace oc
