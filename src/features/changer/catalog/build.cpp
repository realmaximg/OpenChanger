#include "features/changer/catalog/catalog.h"

#include "core/log.h"
#include "sdk/client/CCStrike15ItemDefinition.h"
#include "sdk/client/CEconItemAttributeDefinition.h"
#include "sdk/client/CEconItemDefinition.h"
#include "sdk/client/CEconItemSchema.h"
#include "sdk/client/CEconMusicDefinition.h"
#include "sdk/client/CKeychainDefinition.h"
#include "sdk/client/CPaintKit.h"
#include "sdk/client/CStickerKit.h"
#include "sdk/client/C_EconItemView.h"
#include "sdk/client/static_attrib_t.h"
#include "sdk/filesystem/CFileSystem_Stdio.h"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace oc::changer {

namespace {

struct GameItems {
  std::unordered_map<std::uint16_t, CCStrike15ItemDefinition *> Definitions;
  std::unordered_map<std::uint32_t, CPaintKit *> Paints;
};

std::string text(const char *Value) { return Value != nullptr ? Value : ""; }

template <class T> bool readId(std::int32_t Found, const char *Name, T &Id) {
  if (Found < 0) {
    OC_LOG("catalog", "'{}' is not in the schema", Name);
    return false;
  }
  Id = static_cast<T>(Found);
  return true;
}

bool readIds(CEconItemSchema &Schema, SchemaIds &Ids) {
  const auto Slot = [&Schema](const char *Name, std::uint16_t &Id) {
    return readId(Schema.getLoadoutSlot(Name), Name, Id);
  };
  const auto Team = [&Schema](const char *Name, std::uint8_t &Id) {
    return readId(Schema.getClassUsability(Name), Name, Id);
  };
  const auto Quality = [&Schema](const char *Name, std::int32_t &Id) {
    const std::int32_t Found = Schema.GetQualityIndex(Name);
    return readId(Found > 0 ? Found : -1, Name, Id);
  };
  return Slot(CEconItemSchema::MeleeSlot, Ids.Knife) &&
         Slot(CEconItemSchema::CustomPlayerSlot, Ids.Agent) &&
         Slot(CEconItemSchema::HandsSlot, Ids.Gloves) &&
         Slot(CEconItemSchema::MusicKitSlot, Ids.Music) &&
         Slot(CEconItemSchema::FlairSlot, Ids.Medal) &&
         Team(CEconItemSchema::NoTeam, Ids.AnyTeam) &&
         Team(CEconItemSchema::Terrorists, Ids.Terrorists) &&
         Team(CEconItemSchema::CounterTerrorists, Ids.CounterTerrorists) &&
         Quality(CEconItemSchema::UniqueQuality, Ids.Unique) &&
         Quality(CEconItemSchema::StrangeQuality, Ids.Strange) &&
         Quality(CEconItemSchema::UnusualQuality, Ids.Unusual) &&
         Quality(CEconItemSchema::TournamentQuality, Ids.Tournament);
}

struct AttributeName {
  std::uint16_t *Index;
  std::string Name;
};

std::vector<AttributeName> attributeNames(AttributeIndices &Indices) {
  using Names = CEconItemAttributeDefinition;
  AttributeIndices::KeychainSlot &Keychain = Indices.Keychain;
  std::vector<AttributeName> List{
      {.Index = &Indices.Paint, .Name = Names::PaintKit},
      {.Index = &Indices.Seed, .Name = Names::PaintSeed},
      {.Index = &Indices.Wear, .Name = Names::PaintWear},
      {.Index = &Indices.KillEater, .Name = Names::KillEater},
      {.Index = &Indices.KillEaterType, .Name = Names::KillEaterScoreType},
      {.Index = &Indices.CustomName, .Name = Names::CustomName},
      {.Index = &Indices.TournamentEvent, .Name = Names::TournamentEvent},
      {.Index = &Indices.MusicId, .Name = Names::MusicId},
      {.Index = &Keychain.Id, .Name = Names::KeychainId},
      {.Index = &Keychain.Seed, .Name = Names::KeychainSeed},
      {.Index = &Keychain.OffsetX, .Name = Names::KeychainOffsetX},
      {.Index = &Keychain.OffsetY, .Name = Names::KeychainOffsetY},
      {.Index = &Keychain.OffsetZ, .Name = Names::KeychainOffsetZ},
      {.Index = &Keychain.Highlight, .Name = Names::KeychainHighlight},
  };
  for (std::size_t Slot = 0; Slot < Indices.Sticker.size(); ++Slot) {
    AttributeIndices::StickerSlot &Sticker = Indices.Sticker[Slot];
    const Names::StickerSlotNames Slots = Names::stickerSlotNames(Slot);
    List.insert(List.end(),
                {
                    {.Index = &Sticker.Id, .Name = Slots.Id},
                    {.Index = &Sticker.Wear, .Name = Slots.Wear},
                    {.Index = &Sticker.Scale, .Name = Slots.Scale},
                    {.Index = &Sticker.Rotation, .Name = Slots.Rotation},
                    {.Index = &Sticker.OffsetX, .Name = Slots.OffsetX},
                    {.Index = &Sticker.OffsetY, .Name = Slots.OffsetY},
                    {.Index = &Sticker.Schema, .Name = Slots.Schema},
                });
  }
  return List;
}

bool readAttributes(CEconItemSchema &Schema, AttributeIndices &Indices) {
  using Score = CEconItemAttributeDefinition::KillEaterScore;
  Indices.KillsScore = std::to_underlying(Score::Kills);
  Indices.MvpsScore = std::to_underlying(Score::Mvps);
  bool Found = true;
  for (const AttributeName &Entry : attributeNames(Indices)) {
    CEconItemAttributeDefinition *const Attribute =
        CEconItemSchema::GetAttributeDefinitionByName(&Schema,
                                                      Entry.Name.c_str());
    if (Attribute == nullptr) {
      OC_LOG("catalog", "attribute '{}' is not in the schema", Entry.Name);
      Found = false;
      continue;
    }
    *Entry.Index = Attribute->m_nDefIndex();
  }
  return Found;
}

std::uint8_t teamsOf(CCStrike15ItemDefinition &Def, const SchemaIds &Ids) {
  std::uint8_t Teams = 0;
  for (const std::uint8_t Team : {Ids.Terrorists, Ids.CounterTerrorists}) {
    if (Def.isUsedByClass(Team)) {
      Teams = static_cast<std::uint8_t>(Teams | (1U << Team));
    }
  }
  return Teams;
}

CatalogItem itemOf(std::uint16_t Definition, CCStrike15ItemDefinition &Def,
                   const SchemaIds &Ids) {
  CatalogItem Item{
      .Definition = Definition,
      .Name = text(Def.DefinitionName()),
      .Model = Def.BaseDisplayModel().get(),
      .VoicePrefix = text(Def.VoPrefix()),
      .Slot = static_cast<std::uint16_t>(Def.LoadoutSlot()),
      .Teams = teamsOf(Def, Ids),
      .Rarity = static_cast<std::uint8_t>(
          C_EconItemView::fauxRarity(Definition, Def, nullptr)),
      .Paintable = Def.hasCapability(CEconItemDefinition::ITEM_CAP_PAINTABLE),
      .NameTag = Def.hasCapability(CEconItemDefinition::ITEM_CAP_NAMEABLE),
      .StatTrak =
          Def.hasCapability(CEconItemDefinition::ITEM_CAP_CAN_STATTRACK_SWAP),
      .CharmSlots = static_cast<std::uint8_t>(Def.KeychainSlots()),
  };
  Item.Kind =
      kindOf(Item.Slot, Def.isItemClass(CEconItemDefinition::KnifeClass),
             Item.Paintable, Ids);
  const auto Stickers = static_cast<std::uint8_t>(Def.StickerSlots());
  if (Item.Kind == ItemKind::Agent) {
    Item.PatchSlots = Stickers;
  } else {
    Item.StickerSlots = Stickers;
  }
  return Item;
}

bool hasStaticAttribute(CEconItemDefinition &Def, std::uint16_t Index) {
  return std::ranges::contains(Def.StaticAttributes().span(), Index,
                               &static_attrib_t::iDefIndex);
}

void addSouvenirEvents(Catalog &Cat, CEconItemDefinition &Def) {
  for (const static_attrib_t &Attribute : Def.StaticAttributes().span()) {
    if (Attribute.iDefIndex == Cat.Attributes.TournamentEvent &&
        !std::ranges::contains(Cat.SouvenirEvents, Attribute.m_value)) {
      Cat.SouvenirEvents.push_back(Attribute.m_value);
    }
  }
}

void readItems(CEconItemSchema &Schema, Catalog &Cat, GameItems &Game) {
  Schema.Items().forEach([&](std::int32_t Id, CCStrike15ItemDefinition *Def) {
    if (Def == nullptr) {
      return;
    }
    const auto Definition = static_cast<std::uint16_t>(Id);
    Game.Definitions.emplace(Definition, Def);
    addSouvenirEvents(Cat, *Def);
    if (Def->LoadoutSlot() < 0) {
      return;
    }
    CatalogItem Item = itemOf(Definition, *Def, Cat.Ids);
    if (Item.Kind == ItemKind::Music &&
        hasStaticAttribute(*Def, Cat.Attributes.MusicId)) {
      return;
    }
    Cat.Items.push_back(std::move(Item));
  });
  std::ranges::sort(Cat.SouvenirEvents);
}

void readPaints(CEconItemSchema &Schema, Catalog &Cat, GameItems &Game) {
  Schema.PaintKits().forEach([&](std::int32_t Id, CPaintKit *Paint) {
    if (Paint == nullptr) {
      return;
    }
    Game.Paints.emplace(static_cast<std::uint32_t>(Id), Paint);
    Cat.Paints.push_back(PaintKit{
        .Id = static_cast<std::uint32_t>(Id),
        .Name = Paint->PaintKitName().get(),
        .LegacyModel = Paint->UseLegacyModel(),
        .WearMin = Paint->WearRemapMin(),
        .WearMax = Paint->WearRemapMax(),
    });
  });
}

void readKits(CEconItemSchema &Schema, Catalog &Cat) {
  Schema.StickerKits().forEach([&Cat](std::int32_t Id, CStickerKit *Sticker) {
    if (Sticker == nullptr) {
      return;
    }
    Kit Each{
        .Id = static_cast<std::uint32_t>(Id),
        .Name = Sticker->StickerKitName().get(),
    };
    if (Sticker->isPatch()) {
      Cat.Patches.push_back(std::move(Each));
      return;
    }
    Each.Type = Sticker->type();
    Cat.Stickers.push_back(std::move(Each));
  });
  Schema.KeychainDefinitions().forEach(
      [&Cat](std::int32_t Id, CKeychainDefinition *Keychain) {
        if (Keychain != nullptr) {
          Cat.Keychains.push_back({
              .Id = static_cast<std::uint32_t>(Id),
              .Name = text(Keychain->KeychainName()),
          });
        }
      });
  Schema.MusicDefinitions().forEach(
      [&Cat](std::int32_t Id, CEconMusicDefinition *Music) {
        if (Music != nullptr) {
          Cat.MusicKits.push_back({
              .Id = static_cast<std::uint32_t>(Id),
              .Name = text(Music->MusicName()),
          });
        }
      });
}

void addRarities(Catalog &Cat, const GameItems &Game) {
  for (PaintKit &Paint : Cat.Paints) {
    for (PaintedItem &Item : Paint.Items) {
      Item.Rarity = static_cast<std::uint8_t>(C_EconItemView::fauxRarity(
          Item.Definition, *Game.Definitions.at(Item.Definition),
          Game.Paints.at(Paint.Id)));
    }
  }
}

} // namespace

std::optional<Catalog> buildCatalog() {
  CEconItemSchema *const Schema = CEconItemSchema::GetItemSchema();
  if (Schema == nullptr) {
    OC_LOG("catalog", "no item schema");
    return std::nullopt;
  }
  Catalog Cat{.MaxNameBytes = C_EconItemView::CustomName{}.size() - 1};
  if (!readIds(*Schema, Cat.Ids) || !readAttributes(*Schema, Cat.Attributes)) {
    return std::nullopt;
  }
  GameItems Game;
  readItems(*Schema, Cat, Game);
  readPaints(*Schema, Cat, Game);
  readKits(*Schema, Cat);
  const std::vector<std::string> Previews =
      CFileSystem_Stdio::get()->findPaintPreviews();
  if (Previews.empty()) {
    OC_LOG("catalog", "no paint preview files found");
    return std::nullopt;
  }
  addPreviewPaints(Cat, Previews);
  addRarities(Cat, Game);
  return Cat;
}

} // namespace oc::changer
