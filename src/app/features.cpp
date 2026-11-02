#include "app/features.h"

#include "core/feature.h"
#include "features/changer/Changer.h"

#include <array>
#include <span>

namespace oc {

std::span<Feature *const> features() {
  static const std::array<Feature *, 1> List{
      &changer::Changer::get(),
  };
  return List;
}

} // namespace oc
