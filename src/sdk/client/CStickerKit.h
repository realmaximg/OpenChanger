#pragma once

#include "core/text.h"
#include "sdk/macros.h"
#include "sdk/tier0/KeyValues.h"
#include "sdk/tier1/CUtlString.h"

#include <algorithm>
#include <array>
#include <cstdint>
#include <string>
#include <string_view>

#define PtStickerKitName                                                       \
  "48 8D 15 ? ? ? ? 48 85 C0 48 8B F9 48 8B EB 48 8B CE 48 0F 45 E8 4D 8B E8 " \
  "FF 15 ? ? ? ? 48 85 C0 74 ? 45 33 C9 45 33 C0 48 8B D5 48 8B C8 FF 15 ? ? " \
  "? ? 48 8B E8 48 8B D5 48 8D 4F *"
#define Ptm_pKVItem "48 8B CE FF 15 ? ? ? ? 48 8B E8 48 89 6F *"
#define PtMaterialPath                                                         \
  "48 8D 4B * 4C 89 B4 24 ? ? ? ? 4C 89 BC 24 ? ? ? ? FF 15 ? ? ? ? 48 8B C8 " \
  "48 8D 54 24 40 FF 15"

class CStickerKit {
public:
  SDK_CLASS(CStickerKit);

  static constexpr std::array<std::string_view, 6> Types{
      "paper", "foil", "glitter", "gold", "holo", "graffiti",
  };

  PATTERN_FIELD(Client, CUtlString, StickerKitName, PtStickerKitName);
  PATTERN_FIELD(Client, KeyValues *, m_pKVItem, Ptm_pKVItem);
  PATTERN_FIELD(Client, CUtlString, MaterialPath, PtMaterialPath);

  bool isPatch() {
    const KeyValues *const Item = m_pKVItem();
    return Item != nullptr &&
           KeyValues::FindKey(Item, "patch_material") != nullptr;
  }

  std::string_view type() {
    std::string_view File = MaterialPath().get();
    File = File.substr(File.find_last_of("/\\") + 1);
    File = File.substr(0, File.rfind('.'));
    const std::string Last = oc::lower(File.substr(File.rfind('_') + 1));
    const auto Found = std::ranges::find(Types, Last);
    return Found == Types.end() ? Types.front() : *Found;
  }
};
