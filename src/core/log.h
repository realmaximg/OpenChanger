#pragma once

#ifdef OPENCHANGER_DEV

#include <format>
#include <string>

namespace oc {

void logLine(const char *Part, const std::string &Text);

} // namespace oc

#define OC_LOG(Part, ...) ::oc::logLine(Part, std::format(__VA_ARGS__))

#else

#define OC_LOG(Part, ...) ((void)0)

#endif
