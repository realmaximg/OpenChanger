#include "features/changer/loadout/loadout.h"

#include "features/changer/catalog/catalog.h"

#include <algorithm>
#include <cstdint>
#include <utility>
#include <vector>

namespace oc::changer {

namespace {

constexpr std::uint64_t ItemIdBase = 0x75e'ed00'0000;
constexpr std::uint64_t ItemIdBySlot = 0x10'0000;
constexpr std::uint64_t ItemIdBits = 0xff'ffff;

} // namespace

bool isMedal(const ItemRecord &Record, const SchemaIds &Ids) {
  return Record.Slot == Ids.Medal;
}

bool isEquipped(const ItemRecord &Record, const SchemaIds &Ids) {
  return !isMedal(Record, Ids) || Record.Shown;
}

bool isWeapon(const ItemRecord &Record, const SchemaIds &Ids) {
  return Record.Slot != Ids.Knife && Record.Slot != Ids.Agent &&
         Record.Slot != Ids.Gloves && Record.Slot != Ids.Music &&
         !isMedal(Record, Ids);
}

bool sameKey(const ItemRecord &A, const ItemRecord &B, const SchemaIds &Ids) {
  return ourItemId(A, Ids) == ourItemId(B, Ids);
}

std::uint64_t ourItemId(const ItemRecord &Record, const SchemaIds &Ids) {
  const std::uint64_t Team = std::uint64_t{Record.Team} << 16U;
  if (isMedal(Record, Ids) || isWeapon(Record, Ids)) {
    return ItemIdBase | Team | Record.Definition;
  }
  return ItemIdBase | ItemIdBySlot | Team | Record.Slot;
}

std::uint64_t draftItemId() { return ItemIdBase | ItemIdBits; }

bool isOurItemId(std::uint64_t Id) { return (Id & ~ItemIdBits) == ItemIdBase; }

std::int32_t qualityOf(const ItemRecord &Record, const SchemaIds &Ids) {
  if (Record.Slot == Ids.Knife || Record.Slot == Ids.Gloves) {
    return Ids.Unusual;
  }
  if (Record.SouvenirEvent != 0) {
    return Ids.Tournament;
  }
  return Record.StatTrak ? Ids.Strange : Ids.Unique;
}

void Loadout::put(ItemRecord Record, const SchemaIds &Ids) {
  if (Record.Shown) {
    for (ItemRecord &Other : Items) {
      Other.Shown = false;
    }
  }
  if (ItemRecord *const Existing = findById(ourItemId(Record, Ids), Ids)) {
    *Existing = std::move(Record);
  } else {
    Items.push_back(std::move(Record));
  }
}

void Loadout::remove(const ItemRecord &Record, const SchemaIds &Ids) {
  std::erase_if(Items, [&](const ItemRecord &Other) {
    return sameKey(Other, Record, Ids);
  });
}

const ItemRecord *Loadout::knifeById(std::uint64_t Id,
                                     const SchemaIds &Ids) const {
  const ItemRecord *const Record = findById(Id, Ids);
  return Record != nullptr && Record->Slot == Ids.Knife ? Record : nullptr;
}

const ItemRecord *Loadout::byDefinition(std::uint8_t Team,
                                        std::uint16_t Definition) const {
  const auto It = std::ranges::find_if(Items, [&](const ItemRecord &Record) {
    return Record.Team == Team && Record.Definition == Definition;
  });
  return It == Items.end() ? nullptr : &*It;
}

const ItemRecord *Loadout::find(std::uint8_t Team, std::uint16_t Slot,
                                const SchemaIds &Ids) const {
  const auto It = std::ranges::find_if(Items, [&](const ItemRecord &Record) {
    return Record.Team == Team && Record.Slot == Slot &&
           isEquipped(Record, Ids);
  });
  return It == Items.end() ? nullptr : &*It;
}

const CatalogItem *agentOf(const Loadout &Items, const Catalog &Cat,
                           std::uint8_t Team) {
  const ItemRecord *const Agent = Items.find(Team, Cat.Ids.Agent, Cat.Ids);
  return Agent == nullptr ? nullptr : Cat.item(Agent->Definition);
}

} // namespace oc::changer
