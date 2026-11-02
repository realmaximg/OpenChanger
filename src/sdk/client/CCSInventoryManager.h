#pragma once

#include "sdk/client/CBaseGameSystemFactory.h"
#include "sdk/client/CCSPlayerInventory.h"
#include "sdk/client/CInventoryManager.h"
#include "sdk/client/CPlayerInventory.h"
#include "sdk/client/C_EconItemView.h"
#include "sdk/client/SOID_t.h"
#include "sdk/macros.h"

#include <cstddef>
#include <cstdint>

#define Ptm_pLocalInventory                                                    \
  "45 0F B7 E8 44 0F B7 E2 E8 ? ? ? ? 4C 8B F0 48 89 44 24 28 48 8B B8 * * * " \
  "* 48 85 FF 0F 84"
#define PtAdjustEquipSlots                                                     \
  "48 8B C4 53 48 81 EC ? ? ? ? 48 89 68 08 48 89 78 18 4C 89 60 F0 4C 89 70 " \
  "E8 4C 8B F1 41 0F B6 D0 48 8D 48 B8 81 C2 E3 09 00 00 E8 ? ? ? ? 48 8D 05 " \
  "? ? ? ? 48 C7 44 24 28 7C 01 00 00"
#define PtRemovePendingEquip                                                   \
  "4C 63 91 ? ? ? ? 45 33 C9 4C 8B D9 45 85 D2 7E ? 48 8B 81 ? ? ? ? 0F "      \
  "1F 80 00 00 00 00 66 39 10 75 ? 66 44 39 40 02 74"
#define Ptm_PendingEquipCount                                                  \
  "49 8B BE ? ? ? ? 33 C0 49 63 AE * * * * 48 C1 E5 05 48 03 EF 48 3B FD 0F "  \
  "84"
#define Ptm_pPendingEquips                                                     \
  "49 8B BE * * * * 33 C0 49 63 AE ? ? ? ? 48 C1 E5 05 48 03 EF 48 3B FD 0F "  \
  "84"
#define PtCPendingEquip_m_Slot                                                 \
  "41 89 48 18 0F B7 4F * 41 89 50 10 83 CA 04 41 89 48 1C 48 8B 47 ? 41 89 "  \
  "50 10 49 89 40 20"
#define PtCPendingEquip_m_ItemId                                               \
  "0F B7 4F ? 41 89 50 10 83 CA 04 41 89 48 1C 48 8B 47 * 41 89 50 10 49 89 "  \
  "40 20 8B 47 ?"
#define PtCPendingEquip_Size "3B F0 0F 43 C6 48 83 C7 * 48 3B FD 0F 85"
#define Ptm_DefaultItems "49 69 C7 ? ? ? ? 48 05 * * * * 49 03 C4"
#define PtDefaultItemSize "49 69 C7 * * * * 48 05 ? ? ? ? 49 03 C4"
#define PtDefaultItemSlots "48 63 C2 4C 6B F8 * 48 89 5C 24 40 4C 03 FD"

class CPendingEquip {
public:
  SDK_CLASS(CPendingEquip);

  std::uint16_t m_Class;

  PATTERN_FIELD(Client, std::uint16_t, m_Slot, PtCPendingEquip_m_Slot);
  PATTERN_FIELD(Client, std::uint64_t, m_ItemId, PtCPendingEquip_m_ItemId);
  SDK_PATTERN_LINK(Client, Size, PtCPendingEquip_Size)
};

static_assert(offsetof(CPendingEquip, m_Class) == 0x0);

class CCSInventoryManager : public CInventoryManager {
public:
  SDK_GAME_SYSTEM(CCSInventoryManager, "CCSInventoryManager");

  PATTERN_FUNCTION(Client,
                   bool(CCSInventoryManager *, CCSPlayerInventory *,
                        std::uint8_t),
                   AdjustEquipSlots, PtAdjustEquipSlots);
  PATTERN_FUNCTION(Client,
                   void(CCSInventoryManager *, std::uint16_t, std::uint16_t),
                   RemovePendingEquip, PtRemovePendingEquip);
  PATTERN_FIELD(Client, CCSPlayerInventory *, m_pLocalInventory,
                Ptm_pLocalInventory);
  PATTERN_FIELD(Client, std::int32_t, m_PendingEquipCount,
                Ptm_PendingEquipCount);
  PATTERN_FIELD(Client, CPendingEquip *, m_pPendingEquips, Ptm_pPendingEquips);
  PATTERN_FIELD(Client, C_EconItemView, m_DefaultItems, Ptm_DefaultItems);
  SDK_PATTERN_LINK(Client, DefaultItemSize, PtDefaultItemSize)
  SDK_PATTERN_LINK(Client, DefaultItemSlots, PtDefaultItemSlots)

  CPendingEquip &pendingEquip(std::int32_t Index) {
    return oc::elementAt<CPendingEquip>(m_pPendingEquips(), Index,
                                        CPendingEquip::SizeLink.value());
  }

  void removePendingEquip(CPendingEquip &Entry) {
    RemovePendingEquip(this, Entry.m_Class, Entry.m_Slot());
  }

  std::uint64_t getLocalSteamId() {
    CCSPlayerInventory *const Inventory = m_pLocalInventory();
    return Inventory == nullptr ? 0 : Inventory->m_OwnerID().m_id;
  }

  std::uint32_t getLocalAccountId() {
    return static_cast<std::uint32_t>(getLocalSteamId());
  }

  bool isLocalOwner(const GCSDK::SOID_t &Owner) {
    CCSPlayerInventory *const Inventory = m_pLocalInventory();
    return Inventory != nullptr && Inventory->m_OwnerID().m_id == Owner.m_id &&
           Inventory->m_OwnerID().m_type == Owner.m_type;
  }

  static std::uint16_t loadoutSlotCount() {
    return static_cast<std::uint16_t>(DefaultItemSlotsLink.value());
  }

  C_EconItemView &getDefaultItem(std::uint8_t Team, std::uint16_t Slot) {
    const std::int32_t Index = (Team * loadoutSlotCount()) + Slot;
    return oc::elementAt<C_EconItemView>(&m_DefaultItems(), Index,
                                         DefaultItemSizeLink.value());
  }
};
