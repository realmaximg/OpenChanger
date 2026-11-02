#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <vector>

namespace oc::changer {

enum class ItemKind : std::uint8_t {
  Weapon,
  Knife,
  Gloves,
  Agent,
  Music,
  Medal,
  Other,
};

struct CatalogItem {
  std::uint16_t Definition = 0;
  std::string Name, Model, VoicePrefix;
  ItemKind Kind = ItemKind::Other;
  std::uint16_t Slot = 0;
  std::uint8_t Teams = 0;
  std::uint8_t Rarity = 0;
  bool Paintable = false, NameTag = false, StatTrak = false;
  std::uint8_t StickerSlots = 0, CharmSlots = 0, PatchSlots = 0;
};

struct PaintedItem {
  std::uint16_t Definition = 0;
  std::uint8_t Rarity = 0;
};

struct PaintKit {
  std::uint32_t Id = 0;
  std::string Name;
  bool LegacyModel = false;
  float WearMin = 0, WearMax = 1;
  std::vector<PaintedItem> Items;
};

struct Kit {
  std::uint32_t Id = 0;
  std::string Name, Type;
};

struct AttributeIndices {
  std::uint16_t Paint = 0, Seed = 0, Wear = 0, KillEater = 0, KillEaterType = 0,
                CustomName = 0, TournamentEvent = 0, MusicId = 0;
  std::uint32_t KillsScore = 0, MvpsScore = 0;
  struct StickerSlot {
    std::uint16_t Id = 0, Wear = 0, Scale = 0, Rotation = 0, OffsetX = 0,
                  OffsetY = 0, Schema = 0;
  };
  std::array<StickerSlot, 5> Sticker;
  struct KeychainSlot {
    std::uint16_t Id = 0, Seed = 0, OffsetX = 0, OffsetY = 0, OffsetZ = 0,
                  Highlight = 0;
  } Keychain;
};

struct SchemaIds {
  std::uint16_t Knife = 0, Agent = 0, Gloves = 0, Music = 0, Medal = 0;
  std::uint8_t AnyTeam = 0, Terrorists = 0, CounterTerrorists = 0;
  std::int32_t Unique = 0, Strange = 0, Unusual = 0, Tournament = 0;
};

struct Catalog {
  std::vector<CatalogItem> Items;
  std::vector<PaintKit> Paints;
  std::vector<Kit> Stickers, Patches, Keychains, MusicKits;
  std::vector<std::uint32_t> SouvenirEvents;
  std::size_t MaxNameBytes = 0;
  AttributeIndices Attributes;
  SchemaIds Ids;

  [[nodiscard]] const CatalogItem *item(std::uint16_t Definition) const;
  [[nodiscard]] const CatalogItem *item(std::string_view Name) const;
  [[nodiscard]] const PaintKit *paint(std::uint32_t Id) const;
  [[nodiscard]] std::uint8_t rarity(std::uint16_t Definition,
                                    std::uint32_t Paint) const;
  [[nodiscard]] bool hasLegacyModel(std::uint16_t Definition) const;
};

ItemKind kindOf(std::uint16_t Slot, bool KnifeClass, bool Paintable,
                const SchemaIds &Ids);
void addPreviewPaints(Catalog &Cat, std::span<const std::string> Previews);
std::optional<Catalog> buildCatalog();

} // namespace oc::changer
