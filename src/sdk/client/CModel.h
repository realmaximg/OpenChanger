#pragma once

#include "sdk/macros.h"
#include "sdk/tier1/CBufferString.h"
#include "sdk/tier1/CUtlStringToken.h"

#include <array>
#include <cstdint>

#define PtFindBodyGroupByName                                                  \
  "48 89 5C 24 08 57 48 83 EC 20 33 DB 48 8B C2 48 8B F9 48 85 D2 74 23 38 "   \
  "1A 74 1F 48 C7 C2 FF FF FF FF 48 FF C2 38 1C 10 75 F8 41 B8 26 59 41 31"
#define PtFindBodyGroupChoice                                                  \
  "85 D2 78 4F 3B 91 ? ? ? ? 7D 47 45 8B 00 45 85 C0 74 3F 48 63 C2 33 D2 "    \
  "48 8D 04 80 48 C1 E0 04"
#define PtGetBodyGroupChoiceCount                                              \
  "85 D2 78 1E 3B 91 ? ? ? ? 7D 16 48 63 C2 48 8D 14 80 48 8B 81 ? ? ? ? 48 "  \
  "03 D2 8B 44 D0 10 C3"
#define PtGetBodyGroupChoiceName                                               \
  "40 53 48 83 EC 30 48 63 C2 49 8B D9 49 63 D0 4C 8B D9 4C 8D 14 80 48 8B "   \
  "81 ? ? ? ? 4D 03 D2 4A 8B 44 D0 18"
#define PtUpdateMeshGroupMask                                                  \
  "48 85 C9 74 7E 48 89 6C 24 18 56 41 56 41 57 48 83 EC 20 45 8B F1 41 8B "   \
  "E8 4C 8B FA 48 8B F1 E8"
#define PtThirdPersonDefault                                                   \
  "48 8B 0D ? ? ? ? 8B 05 * * * * 48 85 C9 89 45 F0 48 8B C3 48 0F 45 C1 48 "  \
  "8B CF 48 89 45 F8 E8 ? ? ? ? 8B 15 ? ? ? ? 4C 8D 45 F0 84 C0 74 0F"
#define PtThirdPersonHidden "84 C0 74 0F 48 8B 0D ? ? ? ? 8B 05 * * * * EB"
#define PtFirstPersonDefault                                                   \
  "8B 05 ? ? ? ? EB 41 48 8B 0D ? ? ? ? 8B 05 * * * * 48 85 C9"
#define PtFirstPersonHidden "84 C0 74 2D 48 8B 0D ? ? ? ? 8B 05 * * * *"

class CModel {
public:
  SDK_CLASS(CModel);

  static constexpr std::uint64_t ResourceType = 0x6c646d76;
  static constexpr const char *GlovesBodyGroup = "first_or_third_person";
  static constexpr std::array<const char *, 3> LegacyBodyGroups = {
      "body",
      "silencer",
      "front_mag",
  };

  PATTERN_FUNCTION(Client, std::int32_t(CModel *, const char *),
                   FindBodyGroupByName, PtFindBodyGroupByName);
  PATTERN_FUNCTION(Client,
                   std::int32_t(CModel *, std::int32_t,
                                const CUtlStringToken *),
                   FindBodyGroupChoice, PtFindBodyGroupChoice);
  PATTERN_FUNCTION(Client, std::int32_t(CModel *, std::int32_t),
                   GetBodyGroupChoiceCount, PtGetBodyGroupChoiceCount);
  PATTERN_FUNCTION(Client,
                   void(CModel *, std::int32_t, std::int32_t, CBufferString *),
                   GetBodyGroupChoiceName, PtGetBodyGroupChoiceName);
  PATTERN_FUNCTION(Client,
                   void(CModel *, std::uint64_t *, std::int32_t, std::int32_t),
                   UpdateMeshGroupMask, PtUpdateMeshGroupMask);
  PATTERN_GLOBAL(Client, CUtlStringToken, ThirdPersonDefault,
                 PtThirdPersonDefault);
  PATTERN_GLOBAL(Client, CUtlStringToken, ThirdPersonHidden,
                 PtThirdPersonHidden);
  PATTERN_GLOBAL(Client, CUtlStringToken, FirstPersonDefault,
                 PtFirstPersonDefault);
  PATTERN_GLOBAL(Client, CUtlStringToken, FirstPersonHidden,
                 PtFirstPersonHidden);

  std::int32_t getHiddenGlovesChoice(std::int32_t Group, std::int32_t Choice);
  std::int32_t getLegacyChoice(std::int32_t Group);
};
