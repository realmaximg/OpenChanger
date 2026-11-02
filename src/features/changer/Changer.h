#pragma once

#include "core/feature.h"
#include "core/published.h"
#include "features/changer/catalog/catalog.h"
#include "features/changer/loadout/loadout.h"
#include "features/changer/voice/voices.h"

#include <cstdint>
#include <expected>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace oc::changer {

struct StickerPlacement {
  std::int32_t Schema = 0;
  float OffsetX = 0, OffsetY = 0, Rotation = 0;
};

class Changer final : public Feature {
public:
  static Changer &get();

  [[nodiscard]] std::string_view name() const override { return "changer"; }
  bool start() override;
  void stop() override;
  void onLevelEnd() override;

  std::expected<void, std::string> equip(ItemRecord Record);
  void unequip(const ItemRecord &Record);
  [[nodiscard]] std::vector<bool> inSlot(const Loadout &Items) const;
  std::expected<void, std::string> preview(ItemRecord Record);
  void endPreview();
  void favorite(std::string Key, bool On);
  static std::optional<StickerPlacement> stickerPlacement();
  std::optional<std::uint32_t> countStatTrak(const ItemRecord &Record);

  [[nodiscard]] std::shared_ptr<const Loadout> loadout() const {
    return Current.get();
  }
  [[nodiscard]] const Catalog &catalog() const { return Cat; }
  [[nodiscard]] const Voices &voices() const { return Table; }
  [[nodiscard]] const ItemRecord *draft() const {
    return Draft ? &*Draft : nullptr;
  }

private:
  void save();

  Published<Loadout> Current;
  Catalog Cat;
  Voices Table;
  std::shared_ptr<const Loadout> Saved;
  std::optional<ItemRecord> Draft;
};

} // namespace oc::changer
