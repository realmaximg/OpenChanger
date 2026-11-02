#include "features/changer/loadout/validate.h"

#include "features/changer/catalog/catalog.h"
#include "features/changer/loadout/loadout.h"

#include <algorithm>
#include <bitset>
#include <cstdint>
#include <expected>
#include <format>
#include <string>
#include <string_view>
#include <vector>

namespace oc::changer {

namespace {

using Outcome = std::expected<void, std::string>;
using Check = Outcome (*)(const ItemRecord &, const CatalogItem &,
                          const Catalog &);

bool teamAllowed(const ItemRecord &Record, const CatalogItem &Item,
                 const Catalog &Cat) {
  if (Item.Kind == ItemKind::Music || Item.Kind == ItemKind::Medal) {
    return Record.Team == Cat.Ids.AnyTeam;
  }
  if (Record.Team != Cat.Ids.Terrorists &&
      Record.Team != Cat.Ids.CounterTerrorists) {
    return false;
  }
  return (Item.Teams & (1U << Record.Team)) != 0;
}

Outcome checkIdentity(const ItemRecord &Record, const CatalogItem &Item,
                      const Catalog &Cat) {
  if (Record.Slot != Item.Slot) {
    return std::unexpected("slot does not match the item's slot");
  }
  if (!teamAllowed(Record, Item, Cat)) {
    return std::unexpected("team is not allowed for the item");
  }
  if (Record.Shown && Item.Kind != ItemKind::Medal) {
    return std::unexpected("only a medal can be shown");
  }
  return {};
}

Outcome checkPaint(const ItemRecord &Record, const CatalogItem &Item,
                   const Catalog &Cat) {
  if (Record.Paint == 0) {
    return {};
  }
  const PaintKit *const Paint = Cat.paint(Record.Paint);
  if (Paint == nullptr) {
    return std::unexpected("paint is not in the catalog");
  }
  if (!Item.Paintable) {
    return std::unexpected("item cannot be painted");
  }
  if (!std::ranges::contains(Paint->Items, Record.Definition,
                             &PaintedItem::Definition)) {
    return std::unexpected("paint does not fit the item");
  }
  if (Record.Wear < Paint->WearMin || Record.Wear > Paint->WearMax) {
    return std::unexpected("wear is outside the paint's range");
  }
  return {};
}

template <class Placed>
Outcome checkPlaced(const std::vector<Placed> &Kits, std::uint8_t Slots,
                    const std::vector<Kit> &Known, std::string_view What) {
  std::bitset<256> Used;
  for (const Placed &Each : Kits) {
    if (Each.Slot >= Slots) {
      return std::unexpected(std::format("{} slot is out of range", What));
    }
    if (Used.test(Each.Slot)) {
      return std::unexpected(std::format("{} slot is used twice", What));
    }
    Used.set(Each.Slot);
    if (!std::ranges::contains(Known, Each.Kit, &Kit::Id)) {
      return std::unexpected(std::format("{} is not in the catalog", What));
    }
  }
  return {};
}

Outcome checkStickers(const ItemRecord &Record, const CatalogItem &Item,
                      const Catalog &Cat) {
  if (Record.Stickers.size() > Item.StickerSlots) {
    return std::unexpected("too many stickers");
  }
  return checkPlaced(Record.Stickers, Item.StickerSlots, Cat.Stickers,
                     "sticker");
}

Outcome checkPatches(const ItemRecord &Record, const CatalogItem &Item,
                     const Catalog &Cat) {
  if (Record.Patches.empty()) {
    return {};
  }
  if (Item.Kind != ItemKind::Agent) {
    return std::unexpected("patches are allowed on agents only");
  }
  return checkPlaced(Record.Patches, Item.PatchSlots, Cat.Patches, "patch");
}

Outcome checkCharm(const ItemRecord &Record, const CatalogItem &Item,
                   const Catalog &Cat) {
  if (!Record.Charm) {
    return {};
  }
  if (Item.CharmSlots == 0) {
    return std::unexpected("item cannot have a charm");
  }
  if (!std::ranges::contains(Cat.Keychains, Record.Charm->Kit, &Kit::Id)) {
    return std::unexpected("charm is not in the catalog");
  }
  return {};
}

Outcome checkExtras(const ItemRecord &Record, const CatalogItem &Item,
                    const Catalog &Cat) {
  if (Record.StatTrak && !Item.StatTrak) {
    return std::unexpected("item cannot be StatTrak");
  }
  if (Record.SouvenirEvent != 0 &&
      !std::ranges::contains(Cat.SouvenirEvents, Record.SouvenirEvent)) {
    return std::unexpected("souvenir event is not in the catalog");
  }
  if (Record.SouvenirEvent != 0 &&
      (Item.Kind != ItemKind::Weapon || Record.Paint == 0)) {
    return std::unexpected("only a painted weapon can be a souvenir");
  }
  if (Item.Kind == ItemKind::Music &&
      !std::ranges::contains(Cat.MusicKits, Record.Music, &Kit::Id)) {
    return std::unexpected("music kit is not in the catalog");
  }
  if (!Record.Name.empty() && !Item.NameTag) {
    return std::unexpected("item cannot have a name tag");
  }
  if (Record.Name.size() > Cat.MaxNameBytes) {
    return std::unexpected("name is too long");
  }
  return {};
}

std::string paintKeyOf(const ItemRecord &Record) {
  return std::format("{} {} {:.3f}", Record.Paint, Record.Seed, Record.Wear);
}

Outcome checkPaintKey(const ItemRecord &Record, const Loadout &Others,
                      const SchemaIds &Ids) {
  if (Record.Paint == 0) {
    return {};
  }
  const std::string Key = paintKeyOf(Record);
  for (const ItemRecord &Other : Others.Items) {
    if (Other.Paint != 0 && Other.Definition != Record.Definition &&
        !sameKey(Other, Record, Ids) && paintKeyOf(Other) == Key) {
      return std::unexpected(std::format(
          "paint, seed and wear equal those of definition {}, which would "
          "get one paint; change the seed or the wear",
          Other.Definition));
    }
  }
  return {};
}

} // namespace

Outcome validate(const ItemRecord &Record, const Loadout &Others,
                 const Catalog &Cat) {
  const CatalogItem *const Item = Cat.item(Record.Definition);
  if (Item == nullptr) {
    return std::unexpected("definition is not in the catalog");
  }
  for (const Check Rule : {
           checkIdentity,
           checkPaint,
           checkStickers,
           checkPatches,
           checkCharm,
           checkExtras,
       }) {
    if (Outcome Result = Rule(Record, *Item, Cat); !Result) {
      return Result;
    }
  }
  return checkPaintKey(Record, Others, Cat.Ids);
}

} // namespace oc::changer
