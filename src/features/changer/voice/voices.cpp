#include "features/changer/voice/voices.h"

#include "core/text.h"
#include "features/changer/catalog/catalog.h"
#include "features/changer/loadout/loadout.h"

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

namespace oc::changer {

namespace {

std::string_view speakerOf(std::string_view Line) {
  return Line.substr(0, Line.find('.'));
}

} // namespace

Voices::Voices(const ResponseRules &Rules, SoundHash Hash) {
  for (const auto &[Name, Model] : Rules.VoiceModels) {
    const std::string Voice = lower(Name);
    if (!Model.empty()) {
      ModelRules.push_back({.Model = lower(Model), .Voice = Voice});
    }
    Lines[Voice];
  }
  for (const auto &[Name, Events] : Rules.Groups) {
    const std::string Group = lower(Name);
    for (auto &[Voice, Concepts] : Lines) {
      if (Group.size() <= Voice.size() || !Group.ends_with(Voice)) {
        continue;
      }
      const std::string Concept = Group.substr(0, Group.size() - Voice.size());
      std::vector<std::string> &Said = Concepts[Concept];
      for (const std::string &Event : Events) {
        Said.push_back(lower(Event));
        if (speakerOf(Said.back()) == Voice) {
          ByHash.try_emplace(Hash(Said.back().c_str()),
                             Spoken{
                                 .Voice = Voice,
                                 .Concept = Concept,
                                 .Position = Said.size() - 1,
                             });
        }
      }
    }
  }
}

std::string Voices::voiceOf(const CatalogItem &Agent) const {
  const std::string Speaker =
      lower(Agent.VoicePrefix.empty() ? Agent.Model : Agent.VoicePrefix);
  const ModelRule *Best = nullptr;
  for (const ModelRule &Rule : ModelRules) {
    if (Speaker.contains(Rule.Model) &&
        (Best == nullptr || Rule.Model.size() > Best->Model.size())) {
      Best = &Rule;
    }
  }
  return Best == nullptr ? std::string{} : Best->Voice;
}

bool Voices::speaks(std::uint32_t Sound) const {
  return ByHash.contains(Sound);
}

const std::string *Voices::translate(std::uint32_t Sound,
                                     const std::string &Voice) const {
  const auto Found = ByHash.find(Sound);
  const auto Target = Lines.find(Voice);
  if (Found == ByHash.end() || Target == Lines.end() ||
      Found->second.Voice == Voice) {
    return nullptr;
  }
  const auto Concept = Target->second.find(Found->second.Concept);
  if (Concept == Target->second.end() || Concept->second.empty()) {
    return nullptr;
  }
  const std::vector<std::string> &Said = Concept->second;
  return &Said[Found->second.Position % Said.size()];
}

std::string agentVoice(const Voices &Table, const Loadout &Items,
                       const Catalog &Cat, std::uint8_t Team) {
  const CatalogItem *const Agent = agentOf(Items, Cat, Team);
  return Agent == nullptr ? std::string{} : Table.voiceOf(*Agent);
}

} // namespace oc::changer
