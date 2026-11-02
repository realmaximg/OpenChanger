#pragma once

#include "game/find.h"
#include "game/module.h"
#include "game/pattern.h"
#include "sdk/macros.h"

#include <bit>
#include <cstdint>
#include <optional>
#include <string_view>

#define PtFindChildTraverse                                                    \
  "48 83 EC 28 48 8B 05 ? ? ? ? 48 8D 15 ? ? ? ? 48 8B 48 08 48 8B 01 FF 90 "  \
  "* * * *"
#define PtSetStyleFileSet                                                      \
  "40 53 56 57 48 83 EC ? 48 8B FA 48 89 6C 24 ? 48 8B F1 48 3B 91 A8 01 00 "  \
  "00"
#define PtAddStyleFiles                                                        \
  "40 53 55 41 54 41 57 48 83 EC ? 49 63 E8 41 0F B6 D9 4C 8B E2 4C 8B F9 45 " \
  "85 C0 75 ? 84 DB 74 ? 33 D2 E8 * * * *"
#define PtClearStyleFiles "33 D2 E9 * * * *"

class CUIPanel {
  static std::optional<std::uint32_t>
  findSetStyleFileSetCaller(std::string_view Pattern) {
    return oc::findSlot(oc::GameModule::Panorama, "CUIPanel@panorama",
                        [Pattern](auto Function) {
                          return oc::patternTargetAt(oc::GameModule::Panorama,
                                                     Function, Pattern) ==
                                 std::bit_cast<std::uintptr_t>(
                                     SetStyleFileSetLink.address());
                        });
  }

  static std::optional<std::uint32_t> findAddStyleFiles() {
    return findSetStyleFileSetCaller(PtAddStyleFiles);
  }

  static std::optional<std::uint32_t> findClearStyleFiles() {
    return findSetStyleFileSetCaller(PtClearStyleFiles);
  }

public:
  SDK_CLASS(CUIPanel);

  PATTERN_FUNCTION(Panorama, void(CUIPanel *, void *), SetStyleFileSet,
                   PtSetStyleFileSet);
  PATTERN_VIRTUAL(Client, CUIPanel *(const char *), FindChildTraverse,
                  PtFindChildTraverse);
  VIRTUAL_FIND(bool(const char *const *, std::int32_t, bool), AddStyleFiles,
               findAddStyleFiles);
  VIRTUAL_FIND(void(), ClearStyleFiles, findClearStyleFiles);
};
