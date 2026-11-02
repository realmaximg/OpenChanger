#include "core/log.h"
#include "features/changer/Changer.h"
#include "features/changer/catalog/catalog.h"
#include "features/changer/loadout/loadout.h"
#include "game/GameEventHandler.h"
#include "sdk/client/IGameEvent.h"

#include <cstdint>

namespace oc::changer {

namespace {

void onRoundMvp(IGameEvent &Event);

GameEventHandler Handler{"round_mvp", onRoundMvp};

void onRoundMvp(IGameEvent &Event) {
  if (!Event.isLocalPlayer("userid")) {
    return;
  }
  const GameEventKeySymbol_t MusicKitId("musickitid");
  const GameEventKeySymbol_t MusicKitMvps("musickitmvps");
  Changer &Feature = Changer::get();
  const SchemaIds &Ids = Feature.catalog().Ids;
  const auto Items = Feature.loadout();
  const ItemRecord *const Music = Items->find(Ids.AnyTeam, Ids.Music, Ids);
  if (Music == nullptr) {
    return;
  }
  Event.SetInt(&MusicKitId, static_cast<std::int32_t>(Music->Music));
  const auto Count = Feature.countStatTrak(*Music);
  if (Count) {
    Event.SetInt(&MusicKitMvps, static_cast<std::int32_t>(*Count));
  }
  OC_LOG("stattrak", "round_mvp: music kit {}, mvps {}", Music->Music,
         Count.value_or(0));
}

} // namespace

} // namespace oc::changer
