#pragma once

#include "sdk/macros.h"

#include <array>
#include <cstddef>
#include <cstdint>

using MedalRank_t = std::int32_t;

inline constexpr std::size_t MEDAL_CATEGORY_SEASON_COIN = 5;
inline constexpr std::size_t MEDAL_CATEGORY_COUNT = 6;

class CCSPlayerController_InventoryServices {
public:
  SDK_CLASS(CCSPlayerController_InventoryServices);

  using Ranks = std::array<MedalRank_t, MEDAL_CATEGORY_COUNT>;

  SCHEMA_FIELD(std::uint16_t, m_unMusicID);
  SCHEMA_FIELD(Ranks, m_rank);
};
