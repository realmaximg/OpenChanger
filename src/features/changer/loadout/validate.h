#pragma once

#include "features/changer/catalog/catalog.h"
#include "features/changer/loadout/loadout.h"

#include <expected>
#include <string>

namespace oc::changer {

std::expected<void, std::string>
validate(const ItemRecord &Record, const Loadout &Others, const Catalog &Cat);

} // namespace oc::changer
