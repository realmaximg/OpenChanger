#include "features/changer/Changer.h"
#include "features/changer/catalog/catalog.h"
#include "features/changer/loadout/loadout.h"
#include "game/js.h"
#include "game/panorama.h"

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <expected>
#include <memory>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace oc {

namespace ui {

// NOLINTBEGIN(misc-use-internal-linkage)
enum class KitKind : std::uint8_t { Sticker, Patch, Keychain, Music };

struct Item {
  std::uint16_t Definition = 0, Slot = 0;
  std::uint8_t Teams = 0;
  bool Paintable = false, NameTag = false, StatTrak = false, Legacy = false;
  std::uint8_t StickerSlots = 0, CharmSlots = 0, PatchSlots = 0;
};

struct Paint {
  std::uint32_t Id = 0;
  std::string Name;
  float WearMin = 0, WearMax = 1;
};

struct LoadoutItem {
  std::string Id;
  changer::ItemRecord Record;
  bool InSlot = false;
};
// NOLINTEND(misc-use-internal-linkage)

} // namespace ui

template <> struct JsEnum<changer::ItemKind> {
  static constexpr std::string_view Name = "ItemKind";
  static constexpr std::array<std::string_view, 6> Names{
      "weapon", "knife", "gloves", "agent", "music", "medal",
  };
};

static_assert(JsEnum<changer::ItemKind>::Names.size() ==
              std::to_underlying(changer::ItemKind::Other));

template <> struct JsEnum<ui::KitKind> {
  static constexpr std::string_view Name = "KitKind";
  static constexpr std::array<std::string_view, 4> Names{
      "sticker",
      "patch",
      "keychain",
      "music",
  };
};

namespace {

using changer::Changer;
using changer::ItemKind;
using changer::ItemRecord;
using ui::Item;
using ui::KitKind;
using ui::LoadoutItem;
using ui::Paint;

const changer::Catalog &catalog() { return Changer::get().catalog(); }

std::string outcome(const std::expected<void, std::string> &Result) {
  return Result ? std::string{} : Result.error();
}

void loadoutChanged() { notify("changer", "loadoutChanged"); }

std::string changed(const std::expected<void, std::string> &Result) {
  if (Result) {
    loadoutChanged();
  }
  return outcome(Result);
}

std::vector<Item> items(ItemKind Kind) {
  const changer::Catalog &Cat = catalog();
  std::vector<Item> List;
  for (const changer::CatalogItem &Each : Cat.Items) {
    if (Each.Kind != Kind) {
      continue;
    }
    List.push_back({
        .Definition = Each.Definition,
        .Slot = Each.Slot,
        .Teams = Each.Teams,
        .Paintable = Each.Paintable,
        .NameTag = Each.NameTag,
        .StatTrak = Each.StatTrak,
        .Legacy = Cat.hasLegacyModel(Each.Definition),
        .StickerSlots = Each.StickerSlots,
        .CharmSlots = Each.CharmSlots,
        .PatchSlots = Each.PatchSlots,
    });
  }
  return List;
}

std::vector<Paint> paints(std::uint16_t Definition) {
  std::vector<Paint> List;
  for (const changer::PaintKit &Each : catalog().Paints) {
    if (std::ranges::contains(Each.Items, Definition,
                              &changer::PaintedItem::Definition)) {
      List.push_back({
          .Id = Each.Id,
          .Name = Each.Name,
          .WearMin = Each.WearMin,
          .WearMax = Each.WearMax,
      });
    }
  }
  return List;
}

const std::vector<changer::Kit> &kits(KitKind Kind) {
  const changer::Catalog &Cat = catalog();
  switch (Kind) {
  case KitKind::Sticker:
    return Cat.Stickers;
  case KitKind::Patch:
    return Cat.Patches;
  case KitKind::Keychain:
    return Cat.Keychains;
  case KitKind::Music:
    return Cat.MusicKits;
  }
  std::unreachable();
}

const std::vector<std::uint32_t> &souvenirEvents() {
  return catalog().SouvenirEvents;
}

const changer::SchemaIds &ids() { return catalog().Ids; }

std::vector<LoadoutItem> loadout() {
  const Changer &Feature = Changer::get();
  const std::shared_ptr<const changer::Loadout> Current = Feature.loadout();
  const changer::SchemaIds &Ids = catalog().Ids;
  const std::vector<bool> Slotted = Feature.inSlot(*Current);
  std::vector<LoadoutItem> List;
  List.reserve(Current->Items.size());
  for (std::size_t Index = 0; Index < Current->Items.size(); ++Index) {
    const ItemRecord &Record = Current->Items[Index];
    List.push_back({
        .Id = std::to_string(changer::ourItemId(Record, Ids)),
        .Record = Record,
        .InSlot = Slotted[Index],
    });
  }
  return List;
}

std::string equip(ItemRecord Record) {
  return changed(Changer::get().equip(std::move(Record)));
}

void unequip(const ItemRecord &Record) {
  Changer::get().unequip(Record);
  loadoutChanged();
}

ItemRecord medal(std::uint16_t Definition) {
  const changer::SchemaIds &Ids = catalog().Ids;
  return {.Team = Ids.AnyTeam, .Slot = Ids.Medal, .Definition = Definition};
}

std::string showMedal(std::uint16_t Definition) {
  ItemRecord Record = medal(Definition);
  Record.Shown = true;
  return changed(Changer::get().equip(std::move(Record)));
}

void removeMedal(std::uint16_t Definition) {
  Changer::get().unequip(medal(Definition));
  loadoutChanged();
}

std::string preview(ItemRecord Record) {
  return outcome(Changer::get().preview(std::move(Record)));
}

void endPreview() { Changer::get().endPreview(); }

std::string draftId() { return std::to_string(changer::draftItemId()); }

std::vector<std::string> favorites() {
  return Changer::get().loadout()->Favorites;
}

void favorite(std::string Key, bool On) {
  Changer::get().favorite(std::move(Key), On);
}

JsMethod<&items> Items{"changer", "items", "kind"};
JsMethod<&paints> Paints{"changer", "paints", "definition"};
JsMethod<&kits> Kits{"changer", "kits", "kind"};
JsMethod<&souvenirEvents> SouvenirEvents{"changer", "souvenirEvents"};
JsMethod<&ids> Ids{"changer", "ids"};
JsMethod<&loadout> Loadout{"changer", "loadout"};
JsMethod<&equip> Equip{"changer", "equip", "record"};
JsMethod<&unequip> Unequip{"changer", "unequip", "record"};
JsMethod<&showMedal> ShowMedal{"changer", "showMedal", "definition"};
JsMethod<&removeMedal> RemoveMedal{"changer", "removeMedal", "definition"};
JsMethod<&preview> Preview{"changer", "preview", "record"};
JsMethod<&endPreview> EndPreview{"changer", "endPreview"};
JsMethod<&draftId> DraftId{"changer", "draftId"};
JsMethod<&favorites> Favorites{"changer", "favorites"};
JsMethod<&favorite> Favorite{"changer", "favorite", "key", "on"};
JsMethod<&Changer::stickerPlacement> StickerPlacement{"changer",
                                                      "stickerPlacement"};

} // namespace

} // namespace oc
