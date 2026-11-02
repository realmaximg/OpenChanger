#pragma once

#include "game/find.h"
#include "game/module.h"
#include "game/pattern.h"
#include "sdk/macros.h"

#include <cstddef>
#include <cstdint>
#include <format>
#include <optional>
#include <string>

#define Ptm_nDefIndex "0F B7 41 * C3"

class CEconItemAttributeDefinition {
  static std::optional<std::uint32_t> findDefIndex() {
    const auto Table =
        oc::vtableOf(oc::GameModule::Client, "CEconItemAttributeDefinition");
    if (Table.empty()) {
      return std::nullopt;
    }
    return oc::patternValueAt(oc::GameModule::Client, Table[0], Ptm_nDefIndex);
  }

public:
  SDK_CLASS(CEconItemAttributeDefinition);

  static constexpr const char *PaintKit = "set item texture prefab";
  static constexpr const char *PaintSeed = "set item texture seed";
  static constexpr const char *PaintWear = "set item texture wear";
  static constexpr const char *KillEater = "kill eater";
  static constexpr const char *KillEaterScoreType = "kill eater score type";
  static constexpr const char *CustomName = "custom name attr";
  static constexpr const char *TournamentEvent = "tournament event id";
  static constexpr const char *MusicId = "music id";
  static constexpr const char *KeychainId = "keychain slot 0 id";
  static constexpr const char *KeychainSeed = "keychain slot 0 seed";
  static constexpr const char *KeychainOffsetX = "keychain slot 0 offset x";
  static constexpr const char *KeychainOffsetY = "keychain slot 0 offset y";
  static constexpr const char *KeychainOffsetZ = "keychain slot 0 offset z";
  static constexpr const char *KeychainHighlight = "keychain slot 0 highlight";

  // NOLINTNEXTLINE(performance-enum-size)
  enum class KillEaterScore : std::uint32_t { Kills = 0, Mvps = 1 };

  struct StickerSlotNames {
    std::string Id, Wear, Scale, Rotation, OffsetX, OffsetY, Schema;
  };

  static StickerSlotNames stickerSlotNames(std::size_t Slot) {
    const std::string Prefix = std::format("sticker slot {} ", Slot);
    return {
        .Id = Prefix + "id",
        .Wear = Prefix + "wear",
        .Scale = Prefix + "scale",
        .Rotation = Prefix + "rotation",
        .OffsetX = Prefix + "offset x",
        .OffsetY = Prefix + "offset y",
        .Schema = Prefix + "schema",
    };
  }

  FIELD_FIND(std::uint16_t, m_nDefIndex, findDefIndex);
};
