#pragma once

#include "sdk/client/CEconItemDefinition.h"
#include "sdk/client/CPaintKit.h"
#include "sdk/client/client.h"
#include "sdk/macros.h"

#include <algorithm>
#include <array>
#include <cstdint>
#include <string_view>

class C_EconItemView {
public:
  SDK_CLASS(C_EconItemView);

  using CustomName = std::array<char, 161>;

  static constexpr std::uint16_t FirstNonBaseDefinition = 0x100;

  static constexpr std::uint64_t vanillaItemId(std::uint16_t Definition) {
    return 0xF000'0000'0000'0000 | Definition;
  }

  SCHEMA_FIELD(std::uint16_t, m_iItemDefinitionIndex);
  SCHEMA_FIELD(std::int32_t, m_iEntityQuality);
  SCHEMA_FIELD(std::uint32_t, m_iItemIDHigh);
  SCHEMA_FIELD(std::uint32_t, m_iItemIDLow);
  SCHEMA_FIELD(std::uint32_t, m_iAccountID);
  SCHEMA_FIELD(bool, m_bInitialized);
  SCHEMA_FIELD(bool, m_bDisallowSOC);
  SCHEMA_FIELD(CustomName, m_szCustomName);

  std::uint64_t getItemId() {
    return (std::uint64_t{m_iItemIDHigh()} << 32U) | m_iItemIDLow();
  }

  // NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
  void setView(std::uint16_t Definition, std::uint64_t ItemId,
               std::uint32_t AccountId, std::int32_t Quality,
               std::string_view Name) {
    m_iItemDefinitionIndex() = Definition;
    m_iItemIDHigh() = static_cast<std::uint32_t>(ItemId >> 32U);
    m_iItemIDLow() = static_cast<std::uint32_t>(ItemId);
    m_iAccountID() = AccountId;
    m_iEntityQuality() = Quality;
    m_bInitialized() = true;
    m_bDisallowSOC() = false;
    CustomName &Text = m_szCustomName();
    const std::string_view Kept = Name.substr(0, Text.size() - 1);
    std::ranges::copy(Kept, Text.begin());
    Text[Kept.size()] = '\0';
  }

  static std::int32_t fauxRarity(std::uint16_t DefIndex,
                                 CEconItemDefinition &Def, CPaintKit *Paint) {
    if (Paint != nullptr) {
      return client::EconRarity_CombinedItemAndPaintRarity(
          Def.ItemRarity(), Paint->PaintKitRarity());
    }
    return DefIndex < FirstNonBaseDefinition ? 0 : Def.ItemRarity();
  }
};
