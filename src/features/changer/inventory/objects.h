#pragma once

#include "features/changer/catalog/catalog.h"
#include "features/changer/loadout/loadout.h"

#include <compare>
#include <cstdint>
#include <map>
#include <optional>
#include <span>
#include <string>

namespace oc::changer {

struct SlotKey {
  std::uint8_t Team = 0;
  std::uint16_t Slot = 0;
  auto operator<=>(const SlotKey &) const = default;
};

struct EquipSlot {
  SlotKey Key;
  std::uint64_t ItemId = 0;
  std::uint16_t Definition = 0;
  bool operator==(const EquipSlot &) const = default;
};

struct Objects {
  std::map<std::uint64_t, std::string> Items;
  std::map<SlotKey, std::string> Slots;
};

const EquipSlot *realSlotHolding(std::span<const EquipSlot> Real,
                                 std::uint8_t Team, std::uint16_t Definition);
std::optional<SlotKey> equippedSlot(const ItemRecord &Record,
                                    std::span<const EquipSlot> Real,
                                    const SchemaIds &Ids);
Objects objectsOf(const Loadout &Loadout, const ItemRecord *Draft,
                  std::span<const EquipSlot> Real, const Catalog &Cat,
                  std::uint32_t Account);
std::optional<EquipSlot> realSlotOf(const std::string &Bytes);

} // namespace oc::changer
