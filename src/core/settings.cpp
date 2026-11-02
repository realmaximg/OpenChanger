#include "core/settings.h"

#include "core/feature.h"
#include "core/log.h"

#include <array>
#include <cstddef>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <ios>
#include <iterator>
#include <optional>
#include <string>
#include <system_error>

namespace oc {

namespace {

std::filesystem::path pathOf(const Feature &Owner) {
  return userDirectory() / (std::string(Owner.name()) + ".json");
}

} // namespace

std::filesystem::path userDirectory() {
  std::array<wchar_t, 260> Buffer{};
  std::size_t Length = 0;
  if (_wgetenv_s(&Length, Buffer.data(), Buffer.size(), L"APPDATA") != 0) {
    Buffer[0] = L'\0';
  }
  return std::filesystem::path(Buffer.data()) / L"openchanger";
}

std::optional<std::string> readSettings(const Feature &Owner) {
  std::ifstream File(pathOf(Owner), std::ios::binary);
  if (!File) {
    OC_LOG("settings", "{}.json is missing, using defaults", Owner.name());
    return std::nullopt;
  }
  return std::string(std::istreambuf_iterator<char>(File), {});
}

void writeSettings(const Feature &Owner, const std::string &Json) {
  std::error_code Error;
  std::filesystem::create_directories(userDirectory(), Error);
  if (Error) {
    OC_LOG("settings", "cannot create the user directory: {}", Error.message());
  }
  const auto Final = pathOf(Owner);
  auto Temporary = Final;
  Temporary += L".tmp";
  std::ofstream File(Temporary, std::ios::binary);
  File << Json;
  File.close();
  if (File.fail()) {
    OC_LOG("settings", "cannot write {}.json, keeping the old file",
           Owner.name());
    std::filesystem::remove(Temporary, Error);
    return;
  }
  std::filesystem::rename(Temporary, Final, Error);
  if (Error) {
    OC_LOG("settings", "cannot write {}.json: {}", Owner.name(),
           Error.message());
  }
}

} // namespace oc
