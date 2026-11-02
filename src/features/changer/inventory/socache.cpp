#include "features/changer/inventory/socache.h"

#include "core/log.h"
#include "features/changer/Changer.h"
#include "features/changer/catalog/catalog.h"
#include "features/changer/inventory/objects.h"
#include "sdk/client/CCSInventoryManager.h"
#include "sdk/client/CCSPlayerInventory.h"
#include "sdk/client/CGCClientSharedObjectCache.h"
#include "sdk/client/CSharedObject.h"
#include "sdk/client/CSharedObjectCache.h"
#include "sdk/client/C_EconItemView.h"
#include "sdk/client/SOID_t.h"
#include "sdk/sdl3/SDL.h"

#include <cstdint>
#include <map>
#include <optional>
#include <string>
#include <utility>
#include <vector>

namespace oc::changer {

namespace {

using GCSDK::CGCClientSharedObjectCache;
using GCSDK::k_EEconTypeEquipSlot;
using GCSDK::k_EEconTypeItem;

Objects InCache;
std::map<SlotKey, std::string> Real;

void readRealSlots(CGCClientSharedObjectCache &Cache) {
  for (GCSDK::CSharedObject *const Object :
       Cache.objectsOf(k_EEconTypeEquipSlot)) {
    std::optional<std::string> Bytes = Object->bytes();
    const std::optional<EquipSlot> Slot =
        Bytes ? realSlotOf(*Bytes) : std::nullopt;
    if (Slot) {
      Real.insert_or_assign(Slot->Key, std::move(*Bytes));
    }
  }
}

void uncoverSlots(CGCClientSharedObjectCache &Cache, const Objects &Wanted) {
  for (auto It = InCache.Slots.begin(); It != InCache.Slots.end();) {
    if (Wanted.Slots.contains(It->first)) {
      ++It;
      continue;
    }
    const SlotKey Key = It->first;
    const std::string Ours = std::move(It->second);
    It = InCache.Slots.erase(It);
    const auto Genuine = Real.find(Key);
    if (Genuine == Real.end()) {
      Cache.destroy(k_EEconTypeEquipSlot, Ours);
    } else {
      Cache.put(k_EEconTypeEquipSlot, Genuine->second);
    }
  }
}

void syncItems(CGCClientSharedObjectCache &Cache, const Objects &Wanted) {
  for (const auto &[Id, Bytes] : InCache.Items) {
    if (!Wanted.Items.contains(Id)) {
      Cache.destroy(k_EEconTypeItem, Bytes);
    }
  }
  for (const auto &[Id, Bytes] : Wanted.Items) {
    const auto Old = InCache.Items.find(Id);
    if (Old == InCache.Items.end() || Old->second != Bytes) {
      Cache.put(k_EEconTypeItem, Bytes);
    }
  }
  InCache.Items = Wanted.Items;
}

void coverSlots(CGCClientSharedObjectCache &Cache, const Objects &Wanted) {
  for (const auto &[Key, Bytes] : Wanted.Slots) {
    const auto Old = InCache.Slots.find(Key);
    if (Old != InCache.Slots.end() && Old->second == Bytes) {
      continue;
    }
    Cache.put(k_EEconTypeEquipSlot, Bytes);
    InCache.Slots.insert_or_assign(Key, Bytes);
  }
}

void syncInventory() {
  CCSInventoryManager *const Manager = CCSInventoryManager::get();
  CCSPlayerInventory *const Inventory = Manager->m_pLocalInventory();
  CGCClientSharedObjectCache *const Cache =
      Inventory == nullptr ? nullptr : Inventory->m_pSOCache();
  if (Cache == nullptr) {
    OC_LOG("inventory", "sync: no local inventory cache");
    return;
  }
  readRealSlots(*Cache);
  const Changer &Feature = Changer::get();
  const Objects Wanted =
      objectsOf(*Feature.loadout(), Feature.draft(), realSlots(),
                Feature.catalog(), Manager->getLocalAccountId());
  uncoverSlots(*Cache, Wanted);
  syncItems(*Cache, Wanted);
  coverSlots(*Cache, Wanted);
  OC_LOG("inventory", "sync: {} items, {} slots, {} real; cache {} / {}",
         InCache.Items.size(), InCache.Slots.size(), Real.size(),
         Cache->objectsOf(k_EEconTypeItem).size(),
         Cache->objectsOf(k_EEconTypeEquipSlot).size());
}

} // namespace

void requestSync() {
  SDL::RunOnMainThread([](void *) { syncInventory(); }, nullptr, false);
}

void resubscribed() {
  InCache = {};
  Real.clear();
  requestSync();
}

void realSlotChanged(const GCSDK::SOID_t &Owner, GCSDK::CSharedObject &Object,
                     GCSDK::ESOCacheEvent Event) {
  if (Event != GCSDK::eSOCacheEvent_Incremental ||
      Object.GetTypeID() != k_EEconTypeEquipSlot ||
      !CCSInventoryManager::get()->isLocalOwner(Owner)) {
    return;
  }
  const std::optional<std::string> Bytes = Object.bytes();
  const std::optional<EquipSlot> Slot =
      Bytes ? realSlotOf(*Bytes) : std::nullopt;
  if (!Slot) {
    return;
  }
  const auto Known = Real.find(Slot->Key);
  if (!InCache.Slots.contains(Slot->Key) && Known != Real.end() &&
      Known->second == *Bytes) {
    return;
  }
  OC_LOG("inventory", "real slot team {} slot {} is now definition {}",
         Slot->Key.Team, Slot->Key.Slot, Slot->Definition);
  InCache.Slots.erase(Slot->Key);
  requestSync();
}

std::vector<EquipSlot> realSlots() {
  const SchemaIds &Ids = Changer::get().catalog().Ids;
  CCSInventoryManager *const Manager = CCSInventoryManager::get();
  std::vector<EquipSlot> Slots;
  for (const std::uint8_t Team : {Ids.Terrorists, Ids.CounterTerrorists}) {
    for (std::uint16_t Slot = 0; Slot < CCSInventoryManager::loadoutSlotCount();
         ++Slot) {
      const SlotKey Key{.Team = Team, .Slot = Slot};
      const auto Genuine = Real.find(Key);
      if (Genuine != Real.end()) {
        if (const std::optional<EquipSlot> Held = realSlotOf(Genuine->second)) {
          Slots.push_back(*Held);
        }
        continue;
      }
      const std::uint16_t Definition =
          Manager->getDefaultItem(Team, Slot).m_iItemDefinitionIndex();
      if (Definition != 0) {
        Slots.push_back({
            .Key = Key,
            .ItemId = C_EconItemView::vanillaItemId(Definition),
            .Definition = Definition,
        });
      }
    }
  }
  return Slots;
}

} // namespace oc::changer
