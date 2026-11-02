#include "game/DecodedHandler.h"
#include "game/hook.h"
#include "sdk/client/CEntityInstance.h"
#include "sdk/client/CGameEntitySystem.h"
#include "sdk/networksystem/CFlattenedSerializer.h"
#include "sdk/networksystem/CNetworkFieldChangedEventQueue.h"

#include <cstdint>

namespace oc {

namespace {

// NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
std::uint8_t detour(CFlattenedSerializer *Serializer, void *A2, void *A3,
                    void *A4, std::int32_t Entity, std::int32_t A6,
                    CNetworkFieldChangedEventQueue *Changes,
                    std::uint32_t Flags);

InlineHook<detour> Hook{CFlattenedSerializer::DecodeLink};

// NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
std::uint8_t detour(CFlattenedSerializer *Serializer, void *A2, void *A3,
                    void *A4, std::int32_t Entity, std::int32_t A6,
                    CNetworkFieldChangedEventQueue *Changes,
                    std::uint32_t Flags) {
  const std::uint8_t Result =
      Hook.original(Serializer, A2, A3, A4, Entity, A6, Changes, Flags);
  CGameEntitySystem *const System = CGameEntitySystem::get();
  if (System == nullptr || Entity < 0) {
    return Result;
  }
  if (CEntityInstance *const Decoded =
          System->getEntityByIndex(static_cast<std::uint32_t>(Entity))) {
    dispatchDecoded(*Decoded, Changes);
  }
  return Result;
}

} // namespace

} // namespace oc
