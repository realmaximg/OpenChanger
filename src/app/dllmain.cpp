#include <windows.h>

#include "app/app.h"
#include "app/features.h"
#include "core/feature.h"
#include "core/log.h"
#include "game/hook.h"
#include "game/link.h"
#include "sdk/sdl3/SDL.h"
#include "ui/ui.h"

#include <cstddef>
#include <ranges>
#include <span>

namespace oc::app {

namespace {

bool Running = false;

void stopFeatures(std::span<Feature *const> Started) {
  for (Feature *const Each : std::views::reverse(Started)) {
    Each->stop();
  }
}

void start() {
  const std::size_t Missing = resolveLinks();
#ifdef OPENCHANGER_DEV
  logStart(Missing);
#endif
  if (Missing != 0) {
    OC_LOG("app", "{} links missing, not started", Missing);
    return;
  }
  const std::span<Feature *const> Features = features();
  for (std::size_t Started = 0; Started < Features.size(); ++Started) {
    if (!Features[Started]->start()) {
      OC_LOG("app", "{} did not start", Features[Started]->name());
      stopFeatures(Features.first(Started));
      return;
    }
  }
  Running = true;
  if (!installHooks() || !ui::start()) {
    OC_LOG("app", "hooks or the UI did not start");
    unhook();
    SDL::RunOnMainThread([](void *) { stop(); }, nullptr, false);
  }
}

} // namespace

bool running() { return Running; }

void unhook() {
  ui::stop();
  removeHooks();
}

void stop() {
  if (!Running) {
    return;
  }
  Running = false;
  OC_LOG("app", "stop");
  stopFeatures(features());
}

} // namespace oc::app

extern "C" BOOL WINAPI DllMain([[maybe_unused]] HINSTANCE Instance,
                               DWORD Reason, [[maybe_unused]] LPVOID Reserved) {
  if (Reason == DLL_PROCESS_ATTACH &&
      oc::resolveLink(SDL::RunOnMainThreadLink)) {
    SDL::RunOnMainThread([](void *) { oc::app::start(); }, nullptr, false);
  }
  return TRUE;
}
