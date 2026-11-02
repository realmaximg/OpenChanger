#pragma once

#include <string_view>

namespace oc {

bool startPanorama(std::string_view Css, const char *Script);
void stopPanorama();
void notify(std::string_view Feature, std::string_view Event);

} // namespace oc
