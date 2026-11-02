#include <windows.h>

#include "app/app.h"
#include "core/log.h"
#include "game/module.h"
#include "sdk/client/CCSPlayerController.h"
#include "sdk/client/C_CSPlayerPawn.h"
#include "sdk/sdl3/SDL.h"
#include "ui/ui.h"

#include <bit>
#include <cstddef>
#include <string_view>

namespace oc::app {

namespace {

void logClient() {
  const auto Base = baseOf(GameModule::Client);
  if (Base == 0) {
    OC_LOG("app", "start, {} is not loaded", fileName(GameModule::Client));
    return;
  }
  const auto *const Dos = std::bit_cast<const IMAGE_DOS_HEADER *>(Base);
  const auto *const Headers =
      std::bit_cast<const IMAGE_NT_HEADERS64 *>(Base + Dos->e_lfanew);
  OC_LOG("app", "start, {} stamp={:#x} size={:#x}",
         fileName(GameModule::Client), Headers->FileHeader.TimeDateStamp,
         Headers->OptionalHeader.SizeOfImage);
}

void logLocalPlayer() {
  CCSPlayerController *const Controller = CCSPlayerController::getLocal();
  if (Controller == nullptr) {
    OC_LOG("app", "no local controller");
    return;
  }
  C_CSPlayerPawn *const Pawn = Controller->getPawn();
  OC_LOG("app",
         "local controller={:#x} pawn={:#x} team={} steamid={} "
         "controller class={} pawn class={}",
         Controller->getHandle(), Controller->m_hPlayerPawn().m_Handle,
         Controller->m_iTeamNum(), Controller->m_steamID(),
         Controller->getSchemaClassName(),
         Pawn == nullptr ? std::string_view("none")
                         : Pawn->getSchemaClassName());
}

void reloadUi() {
  if (!running()) {
    return;
  }
  ui::stop();
  if (!ui::start()) {
    OC_LOG("app", "the UI did not start");
  }
}

} // namespace

void logStart(std::size_t Missing) {
  logClient();
  if (Missing == 0) {
    logLocalPlayer();
  }
}

} // namespace oc::app

extern "C" __declspec(dllexport) void OpenChangerDevUnload() {
  SDL::RunOnMainThread([](void *) { oc::app::unhook(); }, nullptr, true);
  SDL::RunOnMainThread([](void *) { oc::app::stop(); }, nullptr, true);
}

extern "C" __declspec(dllexport) void OpenChangerDevReloadUi() {
  SDL::RunOnMainThread([](void *) { oc::app::reloadUi(); }, nullptr, true);
}
