#pragma once

#include "game/module.h"

#include <cstddef>
#include <cstdint>
#include <optional>
#include <span>
#include <string_view>

namespace oc {

std::span<const std::uintptr_t> vtableOf(GameModule Module,
                                         std::string_view Class);

template <class Matches>
std::optional<std::uint32_t> findSlot(GameModule Module, std::string_view Class,
                                      Matches Matching) {
  const auto Table = vtableOf(Module, Class);
  std::optional<std::uint32_t> Found;
  for (std::size_t Slot = 0; Slot < Table.size(); ++Slot) {
    if (!Matching(Table[Slot])) {
      continue;
    }
    if (Found) {
      return std::nullopt;
    }
    Found = static_cast<std::uint32_t>(Slot * sizeof(void *));
  }
  return Found;
}

std::optional<std::uint32_t> findSlotMatching(GameModule Module,
                                              std::string_view Class,
                                              std::string_view Pattern);
std::optional<std::uint32_t> findSlotNamed(GameModule Module,
                                           std::string_view Class,
                                           std::string_view Pattern,
                                           std::string_view Name);

} // namespace oc
