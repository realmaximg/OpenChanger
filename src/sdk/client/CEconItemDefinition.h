#pragma once

#include "sdk/client/static_attrib_t.h"
#include "sdk/macros.h"
#include "sdk/tier1/CUtlString.h"
#include "sdk/tier1/CUtlVector.h"

#include <cstdint>
#include <string_view>

#define PtStaticAttributes "4C 63 6B * 44 3B 6B 58"
#define PtItemRarity                                                           \
  "4C 8D 43 * 48 8B D0 49 8B CD E8 ? ? ? ? 48 8B 4B 08 48 8D 15 ? ? ? ? 45 "   \
  "33 C0"
#define PtBaseDisplayModel                                                     \
  "48 85 C0 74 ? 45 33 C9 45 33 C0 33 D2 48 8B C8 FF 15 ? ? ? ? EB ? 49 8B "   \
  "C4 48 8B D0 48 8D 8B * * * *"
#define PtStickerSlots "41 8B 45 58 89 83 * * * *"
#define PtKeychainSlots "C7 83 * * * * 01 00 00 00 EB ? 44 89 A3 6C 01 00 00"
#define PtCapabilities "48 8D B3 * * * * 45 33 C0"
#define PtItemClassname                                                        \
  "48 8B 4B 08 48 8D 15 ? ? ? ? 48 89 83 * * * * FF 15 ? ? ? ? 48 8D 35 ? ? "  \
  "? ?"
#define PtDefinitionName "48 8B 0F 48 8D 15 ? ? ? ? 45 33 C0 48 89 83 * * * *"

class CEconItemDefinition {
public:
  SDK_CLASS(CEconItemDefinition);

  static constexpr std::uint32_t ITEM_CAP_PAINTABLE = 0x1U;
  static constexpr std::uint32_t ITEM_CAP_NAMEABLE = 0x2U;
  static constexpr std::uint32_t ITEM_CAP_CAN_STATTRACK_SWAP = 0x200000U;
  static constexpr std::string_view KnifeClass = "weapon_knife";

  PATTERN_FIELD(Client, CUtlVector<static_attrib_t>, StaticAttributes,
                PtStaticAttributes);
  PATTERN_FIELD(Client, std::uint8_t, ItemRarity, PtItemRarity);
  PATTERN_FIELD(Client, CUtlString, BaseDisplayModel, PtBaseDisplayModel);
  PATTERN_FIELD(Client, std::int32_t, StickerSlots, PtStickerSlots);
  PATTERN_FIELD(Client, std::int32_t, KeychainSlots, PtKeychainSlots);
  PATTERN_FIELD(Client, std::uint32_t, Capabilities, PtCapabilities);
  PATTERN_FIELD(Client, const char *, ItemClassname, PtItemClassname);
  PATTERN_FIELD(Client, const char *, DefinitionName, PtDefinitionName);

  bool isItemClass(std::string_view Class) {
    const char *const Name = ItemClassname();
    return Name != nullptr && Class == Name;
  }

  bool hasCapability(std::uint32_t Capability) {
    return (Capabilities() & Capability) != 0;
  }
};
