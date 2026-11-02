#pragma once

#include "features/changer/catalog/catalog.h"

#include <algorithm>
#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace oc::changer {

struct Sticker {
  std::uint8_t Slot = 0;
  std::uint32_t Kit = 0;
  float Wear = 0, Scale = 1, Rotation = 0, OffsetX = 0, OffsetY = 0;
  std::int32_t Schema = -1;
  bool operator==(const Sticker &) const = default;
};

struct Patch {
  std::uint8_t Slot = 0;
  std::uint32_t Kit = 0;
  bool operator==(const Patch &) const = default;
};

struct Charm {
  std::uint32_t Kit = 0, Pattern = 0, Highlight = 0;
  float OffsetX = 0, OffsetY = 0, OffsetZ = 0;
  bool operator==(const Charm &) const = default;
};

struct ItemRecord {
  std::uint8_t Team = 0;
  std::uint16_t Slot = 0, Definition = 0;
  std::uint32_t Paint = 0, Seed = 0;
  float Wear = 0;
  bool Legacy = false;
  bool StatTrak = false;
  std::uint32_t StatTrakCount = 0;
  std::string Name;
  std::vector<Sticker> Stickers;
  std::vector<Patch> Patches;
  std::optional<oc::changer::Charm> Charm;
  std::uint32_t SouvenirEvent = 0;
  std::uint32_t Music = 0;
  bool Shown = false;
  bool operator==(const ItemRecord &) const = default;
};

bool isMedal(const ItemRecord &Record, const SchemaIds &Ids);
bool isWeapon(const ItemRecord &Record, const SchemaIds &Ids);
bool isEquipped(const ItemRecord &Record, const SchemaIds &Ids);
bool sameKey(const ItemRecord &A, const ItemRecord &B, const SchemaIds &Ids);
std::uint64_t ourItemId(const ItemRecord &Record, const SchemaIds &Ids);
std::uint64_t draftItemId();
bool isOurItemId(std::uint64_t Id);
std::int32_t qualityOf(const ItemRecord &Record, const SchemaIds &Ids);

struct Loadout {
  std::vector<ItemRecord> Items;
  std::vector<std::string> Favorites;

  void put(ItemRecord Record, const SchemaIds &Ids);
  void remove(const ItemRecord &Record, const SchemaIds &Ids);
  [[nodiscard]] auto *findById(this auto &&Self, std::uint64_t Id,
                               const SchemaIds &Ids) {
    const auto It =
        std::ranges::find_if(Self.Items, [&](const ItemRecord &Record) {
          return ourItemId(Record, Ids) == Id;
        });
    return It == Self.Items.end() ? nullptr : &*It;
  }
  [[nodiscard]] const ItemRecord *knifeById(std::uint64_t Id,
                                            const SchemaIds &Ids) const;
  [[nodiscard]] const ItemRecord *byDefinition(std::uint8_t Team,
                                               std::uint16_t Definition) const;
  [[nodiscard]] const ItemRecord *find(std::uint8_t Team, std::uint16_t Slot,
                                       const SchemaIds &Ids) const;
  bool operator==(const Loadout &) const = default;
};

const CatalogItem *agentOf(const Loadout &Items, const Catalog &Cat,
                           std::uint8_t Team);

} // namespace oc::changer
