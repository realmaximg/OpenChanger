#pragma once

#include "sdk/client/CCStrike15ItemDefinition.h"
#include "sdk/client/CEconItemAttributeDefinition.h"
#include "sdk/client/CEconMusicDefinition.h"
#include "sdk/client/CKeychainDefinition.h"
#include "sdk/client/CPaintKit.h"
#include "sdk/client/CStickerKit.h"
#include "sdk/macros.h"
#include "sdk/tier1/CUtlHashMapLarge.h"
#include "sdk/tier1/CUtlMap.h"
#include "sdk/tier1/CUtlVector.h"

#include <cstdint>

#define PtGetItemSchema                                                        \
  "48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 0F 85 ? ? ? ? 48 89 5C 24 30 B9 10 "  \
  "00 00 00 48 89 7C 24 20 E8 ? ? ? ? 33 FF 48 8B D8 48 85 C0 74 ? 48 8D 05 "  \
  "? ? ? ? 48 89 7B 08 B9 78 09 00 00 48 89 03 E8 ? ? ? ? 48 85 C0 74 ? 48 "   \
  "8B C8 E8 ? ? ? ? 48 8B F8 48 8D 05 ? ? ? ? 48 89 7B 08 48 89 03 48 8B C7"
#define PtItems "48 8B D9 48 8D 54 24 38 48 81 C1 * * * * 41 0F B6 F8"
#define PtGetAttributeDefinitionByName                                         \
  "48 89 5C 24 10 48 89 6C 24 18 57 41 56 41 57 48 83 EC 60 48 8D 05 ? ? ? ? " \
  "48 C7 44 24 28 91 37 00 00"
#define PtStringFieldToInt                                                     \
  "48 89 6C 24 20 41 56 48 83 EC 20 4C 8B 72 08 48 8B E9 48 85 C9 74 ? 80 39 " \
  "00 74 ? 48 89 74 24 38 48 63 32"
#define Ptm_vecClassUsabilityStrings                                           \
  "4D 8D 8F * * * * 41 B8 04 00 00 00 48 8D 05"
#define Ptm_vecLoadoutStrings                                                  \
  "41 B8 3A 00 00 00 48 8D 15 ? ? ? ? 49 8B CF E8 ? ? ? ? 4D 8D 8F * * * * "   \
  "41 B8 3A 00 00 00 48 8D 15 ? ? ? ? 49 8B CF E8 ? ? ? ? 4D 8D 8F ? ? ? ? "   \
  "41 B8 3A"
#define PtGetQualityIndex "4C 8B 00 41 FF 90 * * * * 89 85 D0 00 00 00 85 C0 7F"
#define PtPaintKits                                                            \
  "89 54 24 10 53 48 83 EC 20 48 8B D9 48 8D 54 24 38 48 81 C1 * * * * 4C 8B " \
  "C1 E8 ? ? ? ? 4C 63 C0 44 3B 83 A0 02 00 00"
#define PtStickerKits                                                          \
  "89 54 24 10 53 48 83 EC 20 48 8B D9 48 8D 54 24 38 48 81 C1 * * * * 4C 8B " \
  "C1 E8 ? ? ? ? 4C 63 C0 44 3B 83 C8 02 00 00"
#define PtKeychainDefinitions                                                  \
  "40 53 48 83 EC 20 48 8B D9 89 54 24 38 48 81 C1 * * * * 48 8D 54 24 38 4C " \
  "8B C1 E8 ? ? ? ? 4C 63 C0 44 3B 83 88 04 00 00"
#define PtMusicDefinitions                                                     \
  "40 53 48 83 EC 20 48 8B D9 89 54 24 38 48 81 C1 * * * * 48 8D 54 24 38 4C " \
  "8B C1 E8 ? ? ? ? 4C 63 C0 44 3B 83 B0 04 00 00"

class CEconItemSchema {
public:
  SDK_CLASS(CEconItemSchema);

  using ItemMap = CUtlHashMapLarge<std::int32_t, CCStrike15ItemDefinition *>;
  using PaintKitMap = CUtlMap<std::int32_t, CPaintKit *>;
  using StickerKitMap = CUtlMap<std::int32_t, CStickerKit *>;
  using KeychainMap = CUtlMap<std::int32_t, CKeychainDefinition *>;
  using MusicMap = CUtlMap<std::int32_t, CEconMusicDefinition *>;
  using StringVector = CUtlVector<const char *>;

  static constexpr const char *MeleeSlot = "melee";
  static constexpr const char *CustomPlayerSlot = "customplayer";
  static constexpr const char *HandsSlot = "clothing_hands";
  static constexpr const char *MusicKitSlot = "musickit";
  static constexpr const char *FlairSlot = "flair0";
  static constexpr const char *NoTeam = "noteam";
  static constexpr const char *Terrorists = "terrorists";
  static constexpr const char *CounterTerrorists = "counter-terrorists";
  static constexpr const char *UniqueQuality = "unique";
  static constexpr const char *StrangeQuality = "strange";
  static constexpr const char *UnusualQuality = "unusual";
  static constexpr const char *TournamentQuality = "tournament";

  PATTERN_FUNCTION(Client, CEconItemSchema *(), GetItemSchema, PtGetItemSchema);
  PATTERN_FUNCTION(
      Client, CEconItemAttributeDefinition *(CEconItemSchema *, const char *),
      GetAttributeDefinitionByName, PtGetAttributeDefinitionByName);

  PATTERN_FIELD(Client, ItemMap, Items, PtItems);
  PATTERN_FIELD(Client, PaintKitMap, PaintKits, PtPaintKits);
  PATTERN_FIELD(Client, StickerKitMap, StickerKits, PtStickerKits);
  PATTERN_FIELD(Client, KeychainMap, KeychainDefinitions,
                PtKeychainDefinitions);
  PATTERN_FIELD(Client, MusicMap, MusicDefinitions, PtMusicDefinitions);
  PATTERN_FIELD(Client, StringVector, m_vecClassUsabilityStrings,
                Ptm_vecClassUsabilityStrings);
  PATTERN_FIELD(Client, StringVector, m_vecLoadoutStrings,
                Ptm_vecLoadoutStrings);
  PATTERN_VIRTUAL(Client, std::int32_t(const char *), GetQualityIndex,
                  PtGetQualityIndex);
  PATTERN_FUNCTION(Client, std::int32_t(const char *, StringVector *),
                   StringFieldToInt, PtStringFieldToInt);

  std::int32_t getLoadoutSlot(const char *Name) {
    return StringFieldToInt(Name, &m_vecLoadoutStrings());
  }
  std::int32_t getClassUsability(const char *Name) {
    return StringFieldToInt(Name, &m_vecClassUsabilityStrings());
  }
};
