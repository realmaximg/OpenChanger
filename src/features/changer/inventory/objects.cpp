#include "features/changer/inventory/objects.h"

#include "base_gcmessages.pb.h"
#include "features/changer/catalog/catalog.h"
#include "features/changer/loadout/loadout.h"
#include "openchanger.pb.h"

#include <algorithm>
#include <array>
#include <bit>
#include <cstdint>
#include <optional>
#include <span>
#include <string>
#include <utility>

namespace oc::changer {

namespace {

constexpr std::uint32_t DraftPosition = 0x7FF;

std::string bytesOf(std::uint32_t Bits) {
  const auto Bytes = std::bit_cast<std::array<char, sizeof(Bits)>>(Bits);
  return {Bytes.begin(), Bytes.end()};
}

std::string bytesOf(float Value) {
  return bytesOf(std::bit_cast<std::uint32_t>(Value));
}

void add(CSOEconItem &Item, std::uint16_t Index, std::string Bytes) {
  CSOEconItemAttribute *const Attribute = Item.add_attribute();
  Attribute->set_def_index(Index);
  Attribute->set_value_bytes(std::move(Bytes));
}

void addStickers(CSOEconItem &Item, const ItemRecord &Record,
                 const AttributeIndices &Indices) {
  auto Stickers = Record.Stickers;
  std::ranges::sort(Stickers, {}, &Sticker::Slot);
  for (const Sticker &S : Stickers) {
    if (S.Slot >= Indices.Sticker.size()) {
      continue;
    }
    const AttributeIndices::StickerSlot &Slot = Indices.Sticker[S.Slot];
    const std::uint32_t Schema =
        S.Schema >= 0 ? static_cast<std::uint32_t>(S.Schema) : S.Slot;
    add(Item, Slot.Id, bytesOf(S.Kit));
    add(Item, Slot.Wear, bytesOf(S.Wear));
    add(Item, Slot.Scale, bytesOf(S.Scale));
    add(Item, Slot.Rotation, bytesOf(S.Rotation));
    add(Item, Slot.OffsetX, bytesOf(S.OffsetX));
    add(Item, Slot.OffsetY, bytesOf(S.OffsetY));
    add(Item, Slot.Schema, bytesOf(Schema));
  }
}

void addPatches(CSOEconItem &Item, const ItemRecord &Record,
                const AttributeIndices &Indices) {
  for (const Patch &P : Record.Patches) {
    if (P.Slot < Indices.Sticker.size()) {
      add(Item, Indices.Sticker[P.Slot].Id, bytesOf(P.Kit));
    }
  }
}

void addCharm(CSOEconItem &Item, const Charm &C,
              const AttributeIndices &Indices) {
  const AttributeIndices::KeychainSlot &Keychain = Indices.Keychain;
  add(Item, Keychain.Id, bytesOf(C.Kit));
  add(Item, Keychain.Seed, bytesOf(C.Pattern));
  add(Item, Keychain.OffsetX, bytesOf(C.OffsetX));
  add(Item, Keychain.OffsetY, bytesOf(C.OffsetY));
  add(Item, Keychain.OffsetZ, bytesOf(C.OffsetZ));
  if (C.Highlight != 0) {
    add(Item, Keychain.Highlight, bytesOf(C.Highlight));
  }
}

void addName(CSOEconItem &Item, const std::string &Name, std::uint16_t Index) {
  Item.set_custom_name(Name);
  CAttribute_String Value;
  Value.set_value(Name);
  add(Item, Index, Value.SerializeAsString());
}

void addAttributes(CSOEconItem &Item, const ItemRecord &Record,
                   const Catalog &Cat) {
  const AttributeIndices &Indices = Cat.Attributes;
  if (Record.Paint != 0) {
    add(Item, Indices.Paint, bytesOf(static_cast<float>(Record.Paint)));
    add(Item, Indices.Seed, bytesOf(static_cast<float>(Record.Seed)));
    add(Item, Indices.Wear, bytesOf(Record.Wear));
  }
  if (Record.StatTrak) {
    const std::uint32_t Score =
        Record.Slot == Cat.Ids.Music ? Indices.MvpsScore : Indices.KillsScore;
    add(Item, Indices.KillEater, bytesOf(Record.StatTrakCount));
    add(Item, Indices.KillEaterType, bytesOf(Score));
  }
  if (Record.SouvenirEvent != 0) {
    add(Item, Indices.TournamentEvent, bytesOf(Record.SouvenirEvent));
  }
  addStickers(Item, Record, Indices);
  addPatches(Item, Record, Indices);
  if (Record.Charm) {
    addCharm(Item, *Record.Charm, Indices);
  }
  if (Record.Slot == Cat.Ids.Music) {
    add(Item, Indices.MusicId, bytesOf(Record.Music));
  }
  if (!Record.Name.empty()) {
    addName(Item, Record.Name, Indices.CustomName);
  }
}

// NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
std::string itemOf(const ItemRecord &Record, std::uint64_t Id,
                   std::uint32_t Position, std::optional<SlotKey> Equipped,
                   const Catalog &Cat, std::uint32_t Account) {
  CSOEconItem Item;
  Item.set_id(Id);
  Item.set_account_id(Account);
  Item.set_inventory(Position);
  Item.set_def_index(Record.Definition);
  Item.set_quantity(1);
  Item.set_level(1);
  Item.set_quality(static_cast<std::uint32_t>(qualityOf(Record, Cat.Ids)));
  Item.set_flags(0);
  Item.set_origin(0);
  Item.set_rarity(Cat.rarity(Record.Definition, Record.Paint));
  if (!isMedal(Record, Cat.Ids)) {
    addAttributes(Item, Record, Cat);
  }
  if (Equipped) {
    CSOEconItemEquipped *const State = Item.add_equipped_state();
    State->set_new_class(Equipped->Team);
    State->set_new_slot(Equipped->Slot);
  }
  return Item.SerializeAsString();
}

std::string slotOf(const ItemRecord &Record, SlotKey Key, std::uint32_t Account,
                   const SchemaIds &Ids) {
  CSOEconEquipSlot Slot;
  Slot.set_account_id(Account);
  Slot.set_class_id(Key.Team);
  Slot.set_slot_id(Key.Slot);
  Slot.set_item_id(ourItemId(Record, Ids));
  Slot.set_item_definition(Record.Definition);
  return Slot.SerializeAsString();
}

} // namespace

const EquipSlot *realSlotHolding(std::span<const EquipSlot> Real,
                                 std::uint8_t Team, std::uint16_t Definition) {
  const auto Held = std::ranges::find_if(Real, [&](const EquipSlot &Slot) {
    return Slot.Key.Team == Team && Slot.Definition == Definition;
  });
  return Held == Real.end() ? nullptr : &*Held;
}

std::optional<SlotKey> equippedSlot(const ItemRecord &Record,
                                    std::span<const EquipSlot> Real,
                                    const SchemaIds &Ids) {
  if (!isWeapon(Record, Ids)) {
    if (!isEquipped(Record, Ids)) {
      return std::nullopt;
    }
    return SlotKey{.Team = Record.Team, .Slot = Record.Slot};
  }
  const EquipSlot *const Held =
      realSlotHolding(Real, Record.Team, Record.Definition);
  return Held == nullptr ? std::nullopt : std::optional{Held->Key};
}

Objects objectsOf(const Loadout &Loadout, const ItemRecord *Draft,
                  std::span<const EquipSlot> Real, const Catalog &Cat,
                  std::uint32_t Account) {
  Objects Ours;
  std::uint32_t Position = 0;
  for (const ItemRecord &Record : Loadout.Items) {
    const std::uint64_t Id = ourItemId(Record, Cat.Ids);
    const std::optional<SlotKey> Slot = equippedSlot(Record, Real, Cat.Ids);
    Ours.Items.emplace(Id, itemOf(Record, Id, ++Position, Slot, Cat, Account));
    if (Slot) {
      Ours.Slots.emplace(*Slot, slotOf(Record, *Slot, Account, Cat.Ids));
    }
  }
  if (Draft != nullptr) {
    Ours.Items.emplace(draftItemId(),
                       itemOf(*Draft, draftItemId(), DraftPosition,
                              std::nullopt, Cat, Account));
  }
  return Ours;
}

std::optional<EquipSlot> realSlotOf(const std::string &Bytes) {
  CSOEconEquipSlot Slot;
  if (!Slot.ParseFromString(Bytes) || isOurItemId(Slot.item_id())) {
    return std::nullopt;
  }
  return EquipSlot{
      .Key =
          {
              .Team = static_cast<std::uint8_t>(Slot.class_id()),
              .Slot = static_cast<std::uint16_t>(Slot.slot_id()),
          },
      .ItemId = Slot.item_id(),
      .Definition = static_cast<std::uint16_t>(Slot.item_definition()),
  };
}

} // namespace oc::changer
