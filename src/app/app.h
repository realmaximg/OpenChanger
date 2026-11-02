#pragma once

#include <cstddef>

namespace oc::app {

bool running();
void unhook();
void stop();
void logStart(std::size_t Missing);

} // namespace oc::app
