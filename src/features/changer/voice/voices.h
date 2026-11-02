#pragma once

#include "features/changer/catalog/catalog.h"
#include "features/changer/loadout/loadout.h"

#include <cstddef>
#include <cstdint>
#include <map>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace oc::changer {

struct ResponseRules {
  std::vector<std::pair<std::string, std::vector<std::string>>> Groups;
  std::vector<std::pair<std::string, std::string>> VoiceModels;
};

class Voices {
public:
  using SoundHash = std::uint32_t (*)(const char *);

  Voices() = default;
  Voices(const ResponseRules &Rules, SoundHash Hash);

  [[nodiscard]] std::string voiceOf(const CatalogItem &Agent) const;
  [[nodiscard]] bool speaks(std::uint32_t Sound) const;
  [[nodiscard]] const std::string *translate(std::uint32_t Sound,
                                             const std::string &Voice) const;

private:
  struct Spoken {
    std::string Voice, Concept;
    std::size_t Position = 0;
  };

  struct ModelRule {
    std::string Model, Voice;
  };

  std::map<std::string, std::map<std::string, std::vector<std::string>>> Lines;
  std::unordered_map<std::uint32_t, Spoken> ByHash;
  std::vector<ModelRule> ModelRules;
};

std::string agentVoice(const Voices &Table, const Loadout &Items,
                       const Catalog &Cat, std::uint8_t Team);

} // namespace oc::changer
