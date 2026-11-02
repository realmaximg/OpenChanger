#include "core/log.h"
#include "features/changer/Changer.h"
#include "features/changer/voice/voices.h"
#include "game/GameEventHandler.h"
#include "sdk/client/CCSPlayerController.h"
#include "sdk/client/C_BaseEntity.h"
#include "sdk/client/C_CSPlayerPawn.h"
#include "sdk/client/IGameEvent.h"
#include "sdk/client/client.h"
#include "sdk/scenefilecache/CResponseRulesFileCache.h"
#include "sdk/scenefilecache/CResponseSystem.h"

#include <string>
#include <string_view>

namespace oc::changer {

namespace {

constexpr std::string_view HeGrenade = "hegrenade";
constexpr std::string_view ThrowConcept = "Radio.FireInTheHole";

void onGrenadeThrown(IGameEvent &Event);

GameEventHandler Handler{"grenade_thrown", onGrenadeThrown};

std::string pickLine(const std::string &Voice) {
  return CResponseRulesFileCache::readRules([&Voice](CResponseSystem &System) {
           return System.selectLine(std::string(ThrowConcept) + Voice);
         })
      .value_or(std::string{});
}

void onGrenadeThrown(IGameEvent &Event) {
  if (!Event.isLocalPlayer("userid")) {
    return;
  }
  const GameEventKeySymbol_t Weapon("weapon");
  const char *const Thrown = Event.GetString(&Weapon, "");
  C_CSPlayerPawn *const Pawn = CCSPlayerController::getLocalPawn();
  if (Thrown == nullptr || Thrown != HeGrenade || Pawn == nullptr) {
    return;
  }
  const Changer &Feature = Changer::get();
  const std::string Voice = agentVoice(Feature.voices(), *Feature.loadout(),
                                       Feature.catalog(), Pawn->m_iTeamNum());
  const std::string Line = Voice.empty() ? std::string{} : pickLine(Voice);
  if (Line.empty()) {
    return;
  }
  OC_LOG("voice", "HE throw -> '{}'", Line);
  SoundEventStart Started;
  C_BaseEntity::EmitSound(Pawn, &Started, Line.c_str(), 0.0F, nullptr);
}

} // namespace

} // namespace oc::changer
