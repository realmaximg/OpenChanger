#pragma once

#include "core/feature.h"
#include "core/log.h"

#include <filesystem>
#include <optional>
#include <string>

#include <glaze/core/context.hpp>
#include <glaze/core/opts.hpp>
#include <glaze/core/read.hpp>
#include <glaze/json/read.hpp> // NOLINT(misc-include-cleaner)
#include <glaze/json/write.hpp>

namespace oc {

std::filesystem::path userDirectory();
std::optional<std::string> readSettings(const Feature &Owner);
void writeSettings(const Feature &Owner, const std::string &Json);

template <class T> T loadSettings(const Feature &Owner) {
  const std::optional<std::string> Json = readSettings(Owner);
  if (!Json) {
    return T{};
  }
  T Value{};
  constexpr glz::opts Options{.error_on_unknown_keys = false};
  if (const glz::error_ctx Error = glz::read<Options>(Value, *Json)) {
    OC_LOG("settings", "{}.json is invalid, using defaults: {}", Owner.name(),
           glz::format_error(Error, *Json));
    return T{};
  }
  return Value;
}

template <class T> void saveSettings(const Feature &Owner, const T &Value) {
  std::string Json;
  (void)glz::write_json(Value, Json);
  writeSettings(Owner, Json);
}

} // namespace oc
