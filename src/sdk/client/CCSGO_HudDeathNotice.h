#pragma once

#include "sdk/macros.h"

#include <array>
#include <cstring>
#include <string>
#include <string_view>

#define PtWeaponNameRemap                                                      \
  "4C 8B 05 * * * * 48 85 C0 4C 0F 45 E8 49 8B C5 4D 2B C5"

struct DeathNoticeWeaponName_t {
  const char *m_pszEventName;
  const char *m_pszIconName;
};

class CCSGO_HudDeathNotice {
public:
  SDK_CLASS(CCSGO_HudDeathNotice);

  using WeaponNameRemap_t = std::array<DeathNoticeWeaponName_t, 4>;

  PATTERN_GLOBAL(Client, WeaponNameRemap_t, WeaponNameRemap, PtWeaponNameRemap);

  static constexpr std::string_view WeaponPrefix = "weapon_";

  static const char *remapWeaponName(const char *Name) {
    for (const DeathNoticeWeaponName_t &Entry : WeaponNameRemap()) {
      if (std::strcmp(Entry.m_pszEventName, Name) == 0) {
        return Entry.m_pszIconName;
      }
    }
    return Name;
  }

  static std::string itemName(const char *EventWeapon) {
    return std::string(WeaponPrefix) + remapWeaponName(EventWeapon);
  }

  static const char *eventWeapon(const std::string &ItemName) {
    return ItemName.c_str() +
           (ItemName.starts_with(WeaponPrefix) ? WeaponPrefix.size() : 0);
  }
};
