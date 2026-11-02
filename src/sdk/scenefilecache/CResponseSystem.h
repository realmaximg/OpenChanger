#pragma once

#include "sdk/macros.h"
#include "sdk/scenefilecache/Criteria.h"
#include "sdk/scenefilecache/ResponseGroup.h"
#include "sdk/tier1/CRefCounted.h"
#include "sdk/tier1/CUtlMap.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#define Ptm_Responses                                                          \
  "4C 8D 45 ? 48 8D 54 24 ? 48 8D 4D * E8 ? ? ? ? 83 F8 FF 74 ? 48 8B 44 24 "  \
  "? 48 8D 0D"
#define Ptm_Criteria                                                           \
  "4C 8D 47 ? 48 0F 45 C1 48 8D 54 24 ? 48 8D 4F * 48 89 44 24 ? E8 ? ? ? ? "  \
  "4D 8B 46 08 83 F8 FF"
#define PtSelectWeightedResponseFromResponseGroup                              \
  "4C 89 44 24 18 53 55 56 57 41 55 41 56 41 57 48 81 EC A0 00 00 00 44 0F "   \
  "B7 2A 49 8B F0 48 8B DA 4C 8B F9 66 45 85 ED 0F 84"

class CResponseSystem {
public:
  SDK_CLASS(CResponseSystem);

  using ResponseGroups = CUtlMap<const char *, ResponseGroup *>;
  using CriteriaDict = CUtlMap<const char *, Criteria>;
  using SpeakGroups =
      std::vector<std::pair<std::string, std::vector<std::string>>>;
  using VoiceModels = std::vector<std::pair<std::string, std::string>>;

  static constexpr const char *RulesPath = "scripts/talker/response_rules.vrr";
  static constexpr std::string_view ModelCriterion = "model";
  static constexpr std::string_view VoiceCriterion = "Is";

  PATTERN_FIELD(SceneFileCache, ResponseGroups, m_Responses, Ptm_Responses);
  PATTERN_FIELD(SceneFileCache, CriteriaDict, m_Criteria, Ptm_Criteria);
  PATTERN_FUNCTION(SceneFileCache,
                   std::int32_t(CResponseSystem *, ResponseGroup *, void *),
                   SelectWeightedResponseFromResponseGroup,
                   PtSelectWeightedResponseFromResponseGroup);

  SpeakGroups getSpeakGroups();
  VoiceModels getVoiceModels();
  std::string selectLine(std::string_view GroupName);

  PAD(0x8);
  CRefCounted RefCounted;
};

static_assert(offsetof(CResponseSystem, RefCounted) == 0x8);
