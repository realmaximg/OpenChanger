#pragma once

#include <cstddef>
#include <cstdint>
#include <span>
#include <string_view>

namespace oc {

enum class GameModule : std::uint8_t {
  Client,
  Engine2,
  SchemaSystem,
  NetworkSystem,
  ResourceSystem,
  FileSystem,
  SceneFileCache,
  SoundSystem,
  Tier0,
  SDL3,
  Panorama,
  V8,
};

const char *fileName(GameModule Module);
std::uintptr_t baseOf(GameModule Module);
std::span<const std::byte> sectionOf(GameModule Module, std::string_view Name);
void *createInterface(GameModule Module, const char *Name);
std::uintptr_t exportOf(GameModule Module, std::string_view Name);

} // namespace oc
