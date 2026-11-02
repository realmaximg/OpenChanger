#include "core/log.h"
#include "features/changer/Changer.h"
#include "features/changer/catalog/catalog.h"
#include "features/changer/loadout/loadout.h"
#include "game/GameEventHandler.h"
#include "sdk/client/CCSGO_HudDeathNotice.h"
#include "sdk/client/CCSPlayerController.h"
#include "sdk/client/C_CSPlayerPawn.h"
#include "sdk/client/C_CSWeaponBase.h"
#include "sdk/client/IGameEvent.h"

namespace oc::changer {

namespace {

void onPlayerDeath(IGameEvent &Event);

GameEventHandler Handler{"player_death", onPlayerDeath};

bool killedWith(const ItemRecord &Record, const CatalogItem &Item,
                const SchemaIds &Ids) {
  return Item.Slot == Ids.Knife ? Record.Slot == Ids.Knife
                                : Record.Definition == Item.Definition;
}

void showKnife(IGameEvent &Event, const GameEventKeySymbol_t &WeaponName,
               const ItemRecord &Record, const Catalog &Cat) {
  const CatalogItem *const Knife =
      Record.Slot == Cat.Ids.Knife ? Cat.item(Record.Definition) : nullptr;
  if (Knife != nullptr) {
    Event.SetString(&WeaponName,
                    CCSGO_HudDeathNotice::eventWeapon(Knife->Name));
  }
}

void onPlayerDeath(IGameEvent &Event) {
  if (!Event.isLocalPlayer("attacker")) {
    return;
  }
  const GameEventKeySymbol_t WeaponName("weapon");
  C_CSPlayerPawn *const Pawn = CCSPlayerController::getLocalPawn();
  const char *const Weapon = Event.GetString(&WeaponName, "");
  if (Pawn == nullptr || Weapon == nullptr) {
    return;
  }
  Changer &Feature = Changer::get();
  const SchemaIds &Ids = Feature.catalog().Ids;
  const CatalogItem *const Item =
      Feature.catalog().item(CCSGO_HudDeathNotice::itemName(Weapon));
  if (Item == nullptr) {
    return;
  }
  const auto Items = Feature.loadout();
  for (C_CSWeaponBase *const Held : Pawn->getWeapons()) {
    const ItemRecord *const Record =
        Items->findById(Held->getItem().getItemId(), Ids);
    if (Record != nullptr && killedWith(*Record, *Item, Ids)) {
      showKnife(Event, WeaponName, *Record, Feature.catalog());
      const auto Count = Feature.countStatTrak(*Record);
      OC_LOG("stattrak", "player_death: {} counted {}", Item->Name,
             Count.value_or(0));
      return;
    }
  }
}

} // namespace

} // namespace oc::changer
