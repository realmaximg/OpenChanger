#include "features/changer/Changer.h"

#include "core/log.h"
#include "core/settings.h"
#include "features/changer/catalog/catalog.h"
#include "features/changer/inventory/objects.h"
#include "features/changer/inventory/socache.h"
#include "features/changer/loadout/loadout.h"
#include "features/changer/loadout/validate.h"
#include "features/changer/voice/voices.h"
#include "sdk/client/CUI_Item3dPanel.h"
#include "sdk/engine2/CNetworkClientService.h"
#include "sdk/engine2/CNetworkGameClient.h"
#include "sdk/scenefilecache/CResponseRulesFileCache.h"
#include "sdk/scenefilecache/CResponseSystem.h"
#include "sdk/soundsystem/CSoundOpSystem.h"

#include <algorithm>
#include <cstdint>
#include <expected>
#include <memory>
#include <optional>
#include <string>
#include <utility>
#include <vector>

namespace oc::changer {

namespace {

Loadout validRecords(const Loadout &File, const Catalog &Cat) {
  Loadout Kept{.Favorites = File.Favorites};
  for (const ItemRecord &Record : File.Items) {
    if (const auto Valid = validate(Record, Kept, Cat); !Valid) {
      OC_LOG("changer", "dropped team {} slot {} definition {}: {}",
             Record.Team, Record.Slot, Record.Definition, Valid.error());
      continue;
    }
    Kept.put(Record, Cat.Ids);
  }
  return Kept;
}

std::optional<Voices> readVoices() {
  const std::optional<ResponseRules> Rules =
      CResponseRulesFileCache::readRules([](CResponseSystem &System) {
        return ResponseRules{
            .Groups = System.getSpeakGroups(),
            .VoiceModels = System.getVoiceModels(),
        };
      });
  if (!Rules || Rules->VoiceModels.empty()) {
    return std::nullopt;
  }
  return Voices(*Rules, CSoundOpSystem::getSoundEventHash);
}

void fullUpdate() {
  CNetworkGameClient *const Client =
      CNetworkClientService::get()->m_pNetworkGameClient();
  if (Client != nullptr) {
    CNetworkGameClient::ForceFullUpdate(Client, "openchanger");
  }
}

} // namespace

Changer &Changer::get() {
  static Changer Instance;
  return Instance;
}

bool Changer::start() {
  std::optional<Catalog> Built = buildCatalog();
  if (!Built || Built->Items.empty()) {
    OC_LOG("changer", "no catalog from the item schema");
    return false;
  }
  Cat = std::move(*Built);
  OC_LOG("changer",
         "catalog: {} items ({} medals), {} paints, {} stickers, {} patches, "
         "{} keychains, {} music kits, {} souvenir events",
         Cat.Items.size(),
         std::ranges::count(Cat.Items, ItemKind::Medal, &CatalogItem::Kind),
         Cat.Paints.size(), Cat.Stickers.size(), Cat.Patches.size(),
         Cat.Keychains.size(), Cat.MusicKits.size(), Cat.SouvenirEvents.size());
  std::optional<Voices> Read = readVoices();
  if (!Read) {
    OC_LOG("changer", "no voices from the game's response rules");
    return false;
  }
  Table = std::move(*Read);
  OC_LOG("changer", "voices: {} of {} agents have one",
         std::ranges::count_if(Cat.Items,
                               [this](const CatalogItem &Item) {
                                 return Item.Kind == ItemKind::Agent &&
                                        !Table.voiceOf(Item).empty();
                               }),
         std::ranges::count(Cat.Items, ItemKind::Agent, &CatalogItem::Kind));
  Loadout Kept = validRecords(loadSettings<Loadout>(*this), Cat);
  Current.update([&Kept](Loadout &Value) { Value = std::move(Kept); });
  Saved = Current.get();
  requestSync();
  return true;
}

void Changer::stop() {
  save();
  Draft.reset();
  Current.update([](Loadout &Value) { Value = {}; });
  requestSync();
  fullUpdate();
}

void Changer::onLevelEnd() { save(); }

std::expected<void, std::string> Changer::equip(ItemRecord Record) {
  if (auto Valid = validate(Record, *Current.get(), Cat); !Valid) {
    return Valid;
  }
  Current.update([this, &Record](Loadout &Value) {
    Value.put(std::move(Record), Cat.Ids);
  });
  save();
  requestSync();
  return {};
}

void Changer::unequip(const ItemRecord &Record) {
  Current.update(
      [this, &Record](Loadout &Value) { Value.remove(Record, Cat.Ids); });
  save();
  requestSync();
}

std::vector<bool> Changer::inSlot(const Loadout &Items) const {
  const std::vector<EquipSlot> Real = realSlots();
  std::vector<bool> Slotted;
  Slotted.reserve(Items.Items.size());
  for (const ItemRecord &Record : Items.Items) {
    Slotted.push_back(equippedSlot(Record, Real, Cat.Ids).has_value());
  }
  return Slotted;
}

std::expected<void, std::string> Changer::preview(ItemRecord Record) {
  if (auto Valid = validate(Record, *Current.get(), Cat); !Valid) {
    return Valid;
  }
  Draft = std::move(Record);
  requestSync();
  return {};
}

void Changer::endPreview() {
  Draft.reset();
  requestSync();
}

void Changer::favorite(std::string Key, bool On) {
  Current.update([&Key, On](Loadout &Value) {
    std::erase(Value.Favorites, Key);
    if (On) {
      Value.Favorites.push_back(Key);
    }
  });
  save();
}

std::optional<StickerPlacement> Changer::stickerPlacement() {
  const std::int32_t Schema = CUI_Item3dPanel::StickerSchema();
  if (Schema < 0) {
    return std::nullopt;
  }
  return StickerPlacement{
      .Schema = Schema,
      .OffsetX = CUI_Item3dPanel::StickerOffsetX(),
      .OffsetY = CUI_Item3dPanel::StickerOffsetY(),
      .Rotation = CUI_Item3dPanel::StickerRotation(),
  };
}

std::optional<std::uint32_t> Changer::countStatTrak(const ItemRecord &Record) {
  std::optional<std::uint32_t> Count;
  Current.update([&](Loadout &Value) {
    ItemRecord *const Own = Value.findById(ourItemId(Record, Cat.Ids), Cat.Ids);
    if (Own != nullptr && Own->StatTrak) {
      Count = ++Own->StatTrakCount;
    }
  });
  if (Count) {
    requestSync();
  }
  return Count;
}

void Changer::save() {
  const std::shared_ptr<const Loadout> Now = Current.get();
  if (*Now == *Saved) {
    return;
  }
  saveSettings(*this, *Now);
  Saved = Now;
}

} // namespace oc::changer
