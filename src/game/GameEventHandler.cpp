#include "game/GameEventHandler.h"

#include "sdk/client/IGameEvent.h"

#include <string_view>
#include <vector>

namespace oc {

namespace {

std::vector<const GameEventHandler *> &registry() {
  static std::vector<const GameEventHandler *> Handlers;
  return Handlers;
}

} // namespace

GameEventHandler::GameEventHandler(std::string_view Name,
                                   Callback Handler) noexcept
    : Name(Name), Handler(Handler) {
  registry().push_back(this);
}

void dispatchGameEvent(IGameEvent &Event) {
  const char *const Fired = Event.GetName();
  if (Fired == nullptr) {
    return;
  }
  const std::string_view Name(Fired);
  for (const GameEventHandler *const Entry : registry()) {
    if (Entry->Name == Name) {
      Entry->Handler(Event);
    }
  }
}

} // namespace oc
