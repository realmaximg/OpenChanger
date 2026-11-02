#include "game/module.h"

#include <libhat/process.hpp>

#include <bit>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <span>
#include <string_view>
#include <utility>

namespace oc {

namespace {

std::optional<hat::process::module> loaded(GameModule Module) {
  return hat::process::get_module(fileName(Module));
}

} // namespace

const char *fileName(GameModule Module) {
  switch (Module) {
  case GameModule::Client:
    return "client.dll";
  case GameModule::Engine2:
    return "engine2.dll";
  case GameModule::SchemaSystem:
    return "schemasystem.dll";
  case GameModule::NetworkSystem:
    return "networksystem.dll";
  case GameModule::ResourceSystem:
    return "resourcesystem.dll";
  case GameModule::FileSystem:
    return "filesystem_stdio.dll";
  case GameModule::SceneFileCache:
    return "scenefilecache.dll";
  case GameModule::SoundSystem:
    return "soundsystem.dll";
  case GameModule::Tier0:
    return "tier0.dll";
  case GameModule::SDL3:
    return "SDL3.dll";
  case GameModule::Panorama:
    return "panorama.dll";
  case GameModule::V8:
    return "v8.dll";
  }
  std::unreachable();
}

std::uintptr_t baseOf(GameModule Module) {
  const auto Found = loaded(Module);
  return Found ? Found->address() : 0;
}

std::span<const std::byte> sectionOf(GameModule Module, std::string_view Name) {
  const auto Found = loaded(Module);
  return Found ? Found->get_section_data(Name) : std::span<const std::byte>{};
}

void *createInterface(GameModule Module, const char *Name) {
  using Create = void *(*)(const char *, int *);
  const std::uintptr_t Function = exportOf(Module, "CreateInterface");
  return Function != 0 ? std::bit_cast<Create>(Function)(Name, nullptr)
                       : nullptr;
}

std::uintptr_t exportOf(GameModule Module, std::string_view Name) {
  const auto Found = loaded(Module);
  return Found ? Found->get_symbol(Name) : 0;
}

} // namespace oc
