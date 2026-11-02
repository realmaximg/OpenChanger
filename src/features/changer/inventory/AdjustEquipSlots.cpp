#include "core/log.h"
#include "features/changer/Changer.h"
#include "features/changer/catalog/catalog.h"
#include "features/changer/inventory/objects.h"
#include "features/changer/inventory/socache.h"
#include "features/changer/loadout/loadout.h"
#include "game/hook.h"
#include "sdk/client/CCSInventoryManager.h"
#include "sdk/client/CCSPlayerInventory.h"
#include "sdk/client/C_EconItemView.h"

#include <cstdint>
#include <optional>
#include <span>
#include <vector>

namespace oc::changer {

namespace {

bool detour(CCSInventoryManager *Manager, CCSPlayerInventory *Inventory,
            std::uint8_t Type);

InlineHook<detour> Hook{CCSInventoryManager::AdjustEquipSlotsLink};

std::optional<std::uint64_t> sentItemId(std::uint64_t Id, SlotKey Key,
                                        const Loadout &Loadout,
                                        std::span<const EquipSlot> Real,
                                        const SchemaIds &Ids) {
  if (!isOurItemId(Id)) {
    return Id;
  }
  const ItemRecord *const Record = Loadout.findById(Id, Ids);
  if (Record == nullptr || !isWeapon(*Record, Ids)) {
    return std::nullopt;
  }
  const EquipSlot *const Genuine =
      realSlotHolding(Real, Key.Team, Record->Definition);
  return Genuine == nullptr || Genuine->ItemId == 0
             ? C_EconItemView::vanillaItemId(Record->Definition)
             : Genuine->ItemId;
}

struct Replaced {
  std::uint64_t *Id;
  std::uint64_t Ours;
};

std::vector<Replaced> replaceOurIds(CCSInventoryManager &Manager) {
  const Changer &Feature = Changer::get();
  const auto Items = Feature.loadout();
  const std::vector<EquipSlot> Real = realSlots();
  std::vector<Replaced> Changed;
  for (std::int32_t I = 0; I < Manager.m_PendingEquipCount();) {
    CPendingEquip &Entry = Manager.pendingEquip(I);
    const std::uint64_t Id = Entry.m_ItemId();
    const SlotKey Key{
        .Team = static_cast<std::uint8_t>(Entry.m_Class),
        .Slot = Entry.m_Slot(),
    };
    const std::optional<std::uint64_t> Sent =
        sentItemId(Id, Key, *Items, Real, Feature.catalog().Ids);
    if (!Sent) {
      OC_LOG("inventory", "not sent: team {} slot {} id {:#x}", Key.Team,
             Key.Slot, Id);
      Manager.removePendingEquip(Entry);
      continue;
    }
    OC_LOG("inventory", "send team {} slot {} id {:#x}", Key.Team, Key.Slot,
           *Sent);
    if (*Sent != Id) {
      Entry.m_ItemId() = *Sent;
      Changed.push_back({.Id = &Entry.m_ItemId(), .Ours = Id});
    }
    ++I;
  }
  return Changed;
}

bool detour(CCSInventoryManager *Manager, CCSPlayerInventory *Inventory,
            std::uint8_t Type) {
  const std::vector<Replaced> Changed = replaceOurIds(*Manager);
  const bool Result = Hook.original(Manager, Inventory, Type);
  for (const Replaced &Entry : Changed) {
    *Entry.Id = Entry.Ours;
  }
  return Result;
}

} // namespace

} // namespace oc::changer
