#include "app/features.h"
#include "core/feature.h"
#include "game/hook.h"
#include "sdk/client/IGameSystem.h"

namespace oc {

namespace {

void detour(void *LoopState, bool BackgroundMap);

InlineHook<detour> Hook{IGameSystem::LoopDeactivateAllSystemsLink};

void detour(void *LoopState, bool BackgroundMap) {
  for (Feature *const Each : features()) {
    Each->onLevelEnd();
  }
  Hook.original(LoopState, BackgroundMap);
}

} // namespace

} // namespace oc
