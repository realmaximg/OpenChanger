#include "game/DecodedHandler.h"

#include "sdk/client/CEntityInstance.h"
#include "sdk/networksystem/CNetworkFieldChangedEventQueue.h"

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

void dispatchDecoded(CEntityInstance &Entity,
                     CNetworkFieldChangedEventQueue *Changes) {
  for (const DecodedHandler *const Entry : registry()) {
    Entry->Handler(Entity, Changes);
  }
}

} // namespace oc
