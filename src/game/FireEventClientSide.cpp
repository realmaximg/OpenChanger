#include "game/GameEventHandler.h"
#include "game/hook.h"
#include "sdk/client/CGameEventManager.h"
#include "sdk/client/IGameEvent.h"

namespace oc {

namespace {

bool detour(CGameEventManager *Manager, IGameEvent *Event);

VirtualHook<detour> Hook{CGameEventManager::get,
                         CGameEventManager::FireEventClientSideLink};

bool detour(CGameEventManager *Manager, IGameEvent *Event) {
  if (Event != nullptr) {
    dispatchGameEvent(*Event);
  }
  return Hook.original(Manager, Event);
}

} // namespace

} // namespace oc
