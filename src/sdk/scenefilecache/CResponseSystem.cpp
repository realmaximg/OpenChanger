#include "sdk/scenefilecache/CResponseSystem.h"

#include "core/text.h"
#include "sdk/scenefilecache/Criteria.h"
#include "sdk/scenefilecache/ParserResponse.h"
#include "sdk/scenefilecache/ResponseGroup.h"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

namespace {

using GroupsByName = std::unordered_map<std::string, const ResponseGroup *>;

struct Frame {
  const ResponseGroup *Group;
  std::size_t Next;
};

const ResponseGroup *referenced(const GroupsByName &Groups,
                                const ParserResponse &Response) {
  if (Response.type() != RESPONSE_RESPONSE) {
    return nullptr;
  }
  const auto Found = Groups.find(oc::lower(Response.value));
  return Found == Groups.end() ? nullptr : Found->second;
}

std::vector<std::string> speakLines(const GroupsByName &Groups,
                                    const ResponseGroup &Root) {
  std::vector<std::string> Lines;
  std::vector<Frame> Stack{{.Group = &Root, .Next = 0}};
  while (!Stack.empty()) {
    const auto Responses = Stack.back().Group->group();
    if (Stack.back().Next == Responses.size()) {
      Stack.pop_back();
      continue;
    }
    const ParserResponse &Response = Responses[Stack.back().Next++];
    if (Response.value == nullptr) {
      continue;
    }
    if (Response.type() == RESPONSE_SPEAK) {
      Lines.emplace_back(Response.value);
      continue;
    }
    const ResponseGroup *const Next = referenced(Groups, Response);
    if (Next != nullptr && !std::ranges::contains(Stack, Next, &Frame::Group)) {
      Stack.push_back({.Group = Next, .Next = 0});
    }
  }
  return Lines;
}

} // namespace

CResponseSystem::SpeakGroups CResponseSystem::getSpeakGroups() {
  GroupsByName Groups;
  m_Responses().forEach([&Groups](const char *Name, ResponseGroup *Group) {
    if (Name != nullptr && Group != nullptr) {
      Groups.emplace(oc::lower(Name), Group);
    }
  });
  SpeakGroups Speak;
  m_Responses().forEach([&](const char *Name, ResponseGroup *Group) {
    if (Name == nullptr || Group == nullptr) {
      return;
    }
    std::vector<std::string> Lines = speakLines(Groups, *Group);
    if (!Lines.empty()) {
      Speak.emplace_back(Name, std::move(Lines));
    }
  });
  return Speak;
}

CResponseSystem::VoiceModels CResponseSystem::getVoiceModels() {
  VoiceModels Values;
  m_Criteria().forEach([&Values](const char *Name, const Criteria &Criterion) {
    const char *const Key = Criterion.name();
    if (Name == nullptr || Key == nullptr || Key != ModelCriterion ||
        Criterion.value == nullptr) {
      return;
    }
    const std::string_view Rule(Name);
    if (Rule.size() > VoiceCriterion.size() &&
        Rule.starts_with(VoiceCriterion)) {
      Values.emplace_back(Rule.substr(VoiceCriterion.size()), Criterion.value);
    }
  });
  return Values;
}

std::string CResponseSystem::selectLine(std::string_view GroupName) {
  const std::string Wanted = oc::lower(GroupName);
  ResponseGroup *Group = nullptr;
  m_Responses().forEach([&](const char *Name, ResponseGroup *Value) {
    if (Name != nullptr && oc::lower(Name) == Wanted) {
      Group = Value;
    }
  });
  if (Group == nullptr) {
    return {};
  }
  const std::int32_t Index =
      SelectWeightedResponseFromResponseGroup(this, Group, nullptr);
  if (Index < 0 || Index >= Group->m_Size) {
    return {};
  }
  const ParserResponse &Picked =
      Group->group()[static_cast<std::size_t>(Index)];
  return Picked.type() == RESPONSE_SPEAK && Picked.value != nullptr
             ? std::string(Picked.value)
             : std::string{};
}
