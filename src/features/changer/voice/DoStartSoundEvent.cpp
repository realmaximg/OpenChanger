#include "core/log.h"
#include "features/changer/Changer.h"
#include "features/changer/voice/voices.h"
#include "game/hook.h"
#include "sdk/client/CCSPlayerController.h"
#include "sdk/client/CGameEntitySystem.h"
#include "sdk/client/C_CSPlayerPawn.h"
#include "sdk/client/client.h"
#include "sdk/soundsystem/CSoundOpSystem.h"

#include <cstdint>
#include <string>

namespace oc::changer {

namespace {

SoundEventStart *detour(void *System, SoundEventStart *Result, std::uint64_t A3,
                        std::uint64_t A4, std::uint64_t Hash,
                        std::uint64_t Entity, std::uint64_t A7,
                        std::uint64_t A8, std::uint64_t A9, void *A10);

InlineHook<detour> Hook{client::DoStartSoundEventLink};

// NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
std::uint64_t agentLine(std::uint64_t Hash, std::uint64_t Entity) {
  const Changer &Feature = Changer::get();
  const std::uint32_t Sound = CSoundOpSystem::soundEventOf(Hash);
  if (!Feature.voices().speaks(Sound)) {
    return Hash;
  }
  C_CSPlayerPawn *const Pawn = CCSPlayerController::getLocalPawn();
  if (Pawn == nullptr || CGameEntitySystem::indexOf(Entity) !=
                             CGameEntitySystem::indexOf(Pawn->getHandle())) {
    return Hash;
  }
  const std::string Voice = agentVoice(Feature.voices(), *Feature.loadout(),
                                       Feature.catalog(), Pawn->m_iTeamNum());
  const std::string *const Line = Feature.voices().translate(Sound, Voice);
  if (Line == nullptr) {
    return Hash;
  }
  OC_LOG("voice", "my pawn -> '{}'", *Line);
  return CSoundOpSystem::withSoundEvent(Hash, Line->c_str());
}

SoundEventStart *detour(void *System, SoundEventStart *Result, std::uint64_t A3,
                        std::uint64_t A4, std::uint64_t Hash,
                        std::uint64_t Entity, std::uint64_t A7,
                        std::uint64_t A8, std::uint64_t A9, void *A10) {
  return Hook.original(System, Result, A3, A4, agentLine(Hash, Entity), Entity,
                       A7, A8, A9, A10);
}

} // namespace

} // namespace oc::changer
