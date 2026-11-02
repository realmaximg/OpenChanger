#pragma once

#include "game/module.h"

#include <cstdint>
#include <optional>
#include <string_view>

namespace oc {

std::optional<std::uintptr_t> findPattern(GameModule Module,
                                          std::string_view Pattern);
std::optional<std::uint32_t> patternValueAt(GameModule Module,
                                            std::uintptr_t Address,
                                            std::string_view Pattern);
std::optional<std::uintptr_t> patternTargetAt(GameModule Module,
                                              std::uintptr_t Address,
                                              std::string_view Pattern);

} // namespace oc
