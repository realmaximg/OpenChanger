#include "features/changer/catalog/catalog.h"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <span>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace oc::changer {

const CatalogItem *Catalog::item(std::uint16_t Definition) const {
  const auto It =
      std::ranges::find(Items, Definition, &CatalogItem::Definition);
  return It == Items.end() ? nullptr : &*It;
}

const CatalogItem *Catalog::item(std::string_view Name) const {
  const auto It = std::ranges::find(Items, Name, &CatalogItem::Name);
  return It == Items.end() ? nullptr : &*It;
}

const PaintKit *Catalog::paint(std::uint32_t Id) const {
  const auto It = std::ranges::find(Paints, Id, &PaintKit::Id);
  return It == Paints.end() ? nullptr : &*It;
}

// NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
std::uint8_t Catalog::rarity(std::uint16_t Definition,
                             std::uint32_t Paint) const {
  if (const PaintKit *const Kit = Paint == 0 ? nullptr : paint(Paint)) {
    const auto Painted =
        std::ranges::find(Kit->Items, Definition, &PaintedItem::Definition);
    if (Painted != Kit->Items.end()) {
      return Painted->Rarity;
    }
  }
  const CatalogItem *const Item = item(Definition);
  return Item == nullptr ? 0 : Item->Rarity;
}

bool Catalog::hasLegacyModel(std::uint16_t Definition) const {
  return std::ranges::any_of(Paints, [Definition](const PaintKit &Paint) {
    return Paint.LegacyModel && std::ranges::contains(Paint.Items, Definition,
                                                      &PaintedItem::Definition);
  });
}

ItemKind kindOf(std::uint16_t Slot, bool KnifeClass, bool Paintable,
                const SchemaIds &Ids) {
  if (Slot == Ids.Medal) {
    return ItemKind::Medal;
  }
  if (Slot == Ids.Music) {
    return ItemKind::Music;
  }
  if (Slot == Ids.Agent) {
    return ItemKind::Agent;
  }
  if (Slot == Ids.Gloves) {
    return ItemKind::Gloves;
  }
  if (Slot == Ids.Knife) {
    return KnifeClass ? ItemKind::Knife : ItemKind::Other;
  }
  return Paintable ? ItemKind::Weapon : ItemKind::Other;
}

void addPreviewPaints(Catalog &Cat, std::span<const std::string> Previews) {
  std::unordered_map<std::string_view, std::uint16_t> Items;
  for (const CatalogItem &Item : Cat.Items) {
    Items.emplace(Item.Name, Item.Definition);
  }
  std::unordered_map<std::string_view, PaintKit *> Paints;
  for (PaintKit &Paint : Cat.Paints) {
    Paints.emplace(Paint.Name, &Paint);
  }
  for (const std::string_view Name : Previews) {
    for (std::size_t Split = Name.find('_'); Split != std::string_view::npos;
         Split = Name.find('_', Split + 1)) {
      const auto Item = Items.find(Name.substr(0, Split));
      const auto Paint = Paints.find(Name.substr(Split + 1));
      if (Item == Items.end() || Paint == Paints.end()) {
        continue;
      }
      std::vector<PaintedItem> &Fits = Paint->second->Items;
      if (!std::ranges::contains(Fits, Item->second,
                                 &PaintedItem::Definition)) {
        Fits.push_back(PaintedItem{.Definition = Item->second});
      }
    }
  }
}

} // namespace oc::changer
