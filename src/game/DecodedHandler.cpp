#include "game/DecodedHandler.h"

#include "sdk/client/CEntityInstance.h"
#include "sdk/client/CGameEntitySystem.h"
#include "sdk/networksystem/CNetworkFieldChangedEventQueue.h"

#include <cstdint>
#include <vector>

namespace oc {

namespace {

std::vector<const DecodedHandler *> &registry() {
  static std::vector<const DecodedHandler *> Handlers;
  return Handlers;
}

} // namespace

DecodedHandler::DecodedHandler(Callback Handler) noexcept : Handler(Handler) {
  registry().push_back(this);
}

void dispatchDecoded(std::int32_t Index,
                     CNetworkFieldChangedEventQueue *Changes) {
  CGameEntitySystem *const System = CGameEntitySystem::get();
  if (System == nullptr || Index < 0) {
    return;
  }
  CEntityInstance *const Entity =
      System->getEntityByIndex(static_cast<std::uint32_t>(Index));
  if (Entity == nullptr) {
    return;
  }
  for (const DecodedHandler *const Entry : registry()) {
    Entry->Handler(*Entity, Changes);
  }
}

} // namespace oc
