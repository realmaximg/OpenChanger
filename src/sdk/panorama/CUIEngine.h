#pragma once

#include "game/find.h"
#include "game/module.h"
#include "game/pattern.h"
#include "sdk/macros.h"
#include "sdk/v8/Context.h"
#include "sdk/v8/FunctionTemplate.h"
#include "sdk/v8/Isolate.h"
#include "sdk/v8/Local.h"
#include "sdk/v8/Value.h"

#include <bit>
#include <cstdint>
#include <optional>
#include <string_view>

#define PtRegisterJSGlobalFunction                                             \
  "48 8D 15 ? ? ? ? 48 8B 01 FF 90 * * * * 48 8B 0D ? ? ? ? 48 8D 15 ? ? ? ? " \
  "45 33 C9 C6 44 24 20 02"
#define PtGetMainContext                                                       \
  "48 8D 4C 24 60 E8 ? ? ? ? 48 8B 0D ? ? ? ? 48 8B 01 FF 90 * * * * 48 8B "   \
  "0D ? ? ? ? 48 8B F8"
#define PtGetIsolate                                                           \
  "E8 ? ? ? ? 48 8B 0D ? ? ? ? E8 ? ? ? ? 48 8B 0D ? ? ? ? 48 8B 01 "          \
  "FF 90 * * * * 48 8B C8 48 8B D8"
#define PtUnregisterJSGenericCallback                                          \
  "48 8B 06 48 8B CE 8B 13 FF 90 * * * * 48 8D 5B 04 48 83 EF 01 75"
#define PtGetLayoutManager                                                     \
  "48 8B 0D ? ? ? ? 48 8B 01 FF 90 * * * * 4C 8B C3 48 8D 54 24 ? 48 8B C8 E8"
#define PtRunScript                                                            \
  "4C 89 4C 24 20 4C 89 44 24 18 48 89 54 24 10 55 53 56 57 41 54 41 56 "      \
  "41 57 48 8D 6C 24 ? 48 81 EC ? ? ? ? 48 8D 05 ? ? ? ? 48 C7 44 24 38 "      \
  "? ? ? ? 48 89 44 24 30 4D 8B E0 0F 10 44 24 30 48 8D 05 ? ? ? ? 48 8B "     \
  "DA 48 89 44 24 40 4C 8D 45 ? F2 0F 10 4C 24 40 48 8D 15 ? ? ? ? 4C 8B "     \
  "F9 F2 0F 11 4D ? 48 8D 0D * * * *"
#define PtInvokeJSGenericCallback                                              \
  "89 54 24 10 4C 8B DC 53 55 56 41 54 41 55 48 81 EC ? ? ? ? 49 C7 43 ? "     \
  "? ? ? ? 48 8D 05 ? ? ? ? 49 89 43 ? 48 8D 15 ? ? ? ? 48 8D 05 ? ? ? ? 45 "  \
  "8B E8 49 89 43 ? 4D 8D 43 ? 0F 10 44 24 ? 48 8B E9 48 8D 0D * * * *"
#define PtRegisterJSGenericCallback                                            \
  "48 8B C4 48 89 50 10 55 41 56 41 57 48 8B EC 48 83 EC ? 48 89 58 08 4C "    \
  "8B F9 48 89 70 18 BA 18 00 00 00 48 89 78 E0 49 8B D8 4C 89 60 D8 48 8B "   \
  "05 ? ? ? ? 48 8B 08 48 8B 01 FF 50 08 33 FF 4C 8B F0 48 85 C0 74 ? 41 "     \
  "8B B7 ? ? ? ? 4C 8B 65 28 8D 46 01 41 89 87 ? ? ? ? 48 85 DB 75 ? 8B C7 "   \
  "49 89 06 4D 89 66 08 41 89 76 10 EB ? 48 8B 13 49 8B 8F ? ? ? ? FF 15 * "   \
  "* * *"
#define PtMakeSymbol                                                           \
  "40 55 56 48 83 EC ? 48 63 EA 49 8B F0 4D 85 C0 0F 84 ? ? ? ? 41 80 38 00 "  \
  "0F 84 ? ? ? ? 8B 05 ? ? ? ? 48 89 5C 24 ? 48 89 7C 24 ? C6 44 24 ? 00 0F "  \
  "BA E0 1A 72 ? 0F BA E0 1C 73 ? A9 FF FF FF 01 75 ? E8 ? ? ? ? 48 8D 05 ? "  \
  "? ? ? 48 6B DD 68 48 03 D8 48 8D 4B 60 E8 ? ? ? ? 4C 8D 4C 24 ? 4C 8B C6 "  \
  "48 8D 54 24 ? 48 8B CB FF 15 ? ? ? ? 48 8D 4B 60 E8 ? ? ? ? 0F B7 44 24 ? " \
  "B9 FF FF 00 00 48 8B 7C 24 ? 48 8B 5C 24 ? 66 3B C1 75 ? 48 8D 05 ? ? ? ? " \
  "C7 44 24 ? ? ? ? ? 48 89 44 24 ? 48 8D 15 ? ? ? ? 48 8D 05 * * * *"
#define PtPurgeUnusedFiles                                                     \
  "40 53 48 83 EC 20 48 8B D9 48 8B 89 * * * * E8 ? ? ? ? 48 8B 8B F8 0B 00 "  \
  "00 48 83 C4 20 5B E9"

class CUIPanel;
class CLayoutManager;

class CUIEngine {
  static constexpr std::string_view VtableName = "CUIEngineSource2@panorama";
  static constexpr std::int32_t PathSymbols = 1;

  static std::optional<std::uint32_t> findByName(std::string_view Pattern,
                                                 std::string_view Name) {
    return oc::findSlotNamed(oc::GameModule::Panorama, VtableName, Pattern,
                             Name);
  }

  static std::optional<std::uint32_t> findRunScript() {
    return findByName(PtRunScript, "CUIEngine::RunScript (compile+run)");
  }

  static std::optional<std::uint32_t> findInvokeJSGenericCallback() {
    return findByName(PtInvokeJSGenericCallback,
                      "CUIEngine::InvokeJSGenericCallback");
  }

  static std::optional<std::uint32_t> findMakeSymbol() {
    return findByName(PtMakeSymbol, "panorama::InternalMakeSymbol");
  }

  static std::optional<std::uint32_t> findRegisterJSGenericCallback() {
    const std::uintptr_t Globalize = oc::exportOf(
        oc::GameModule::V8, "?GlobalizeReference@api_internal@v8@@YAPEA_"
                            "KPEAVIsolate@internal@2@_K@Z");
    if (Globalize == 0) {
      return std::nullopt;
    }
    return oc::findSlot(
        oc::GameModule::Panorama, VtableName, [Globalize](auto Function) {
          const auto Import = oc::patternTargetAt(
              oc::GameModule::Panorama, Function, PtRegisterJSGenericCallback);
          return Import &&
                 *std::bit_cast<const std::uintptr_t *>(*Import) == Globalize;
        });
  }

  static std::optional<std::uint32_t> findPurgeUnusedFiles() {
    return oc::findSlotMatching(oc::GameModule::Panorama, VtableName,
                                PtPurgeUnusedFiles);
  }

public:
  SDK_CLASS(CUIEngine);

  PATTERN_VIRTUAL(Client, v8::Isolate *(), GetIsolate, PtGetIsolate);
  PATTERN_VIRTUAL(Client, v8::Global<v8::Context> *(), GetMainContext,
                  PtGetMainContext);
  PATTERN_VIRTUAL(Client,
                  void(const char *, v8::Local<v8::FunctionTemplate> *, bool),
                  RegisterJSGlobalFunction, PtRegisterJSGlobalFunction);
  PATTERN_VIRTUAL(Panorama, void(std::int32_t), UnregisterJSGenericCallback,
                  PtUnregisterJSGenericCallback);
  PATTERN_VIRTUAL(Panorama, CLayoutManager *(), GetLayoutManager,
                  PtGetLayoutManager);
  VIRTUAL_FIND(void(CUIPanel *, const char *, const char *, std::uint64_t),
               RunScript, findRunScript);
  VIRTUAL_FIND(std::int32_t(CUIPanel *, v8::Local<v8::Value>),
               RegisterJSGenericCallback, findRegisterJSGenericCallback);
  VIRTUAL_FIND(bool(std::int32_t, std::int32_t, v8::Local<v8::Value> *,
                    v8::Local<v8::Value> *),
               InvokeJSGenericCallback, findInvokeJSGenericCallback);
  VIRTUAL_FIND(std::uint16_t(std::int32_t, const char *), MakeSymbol,
               findMakeSymbol);
  VIRTUAL_FIND(void(), PurgeUnusedFiles, findPurgeUnusedFiles);

  std::uint16_t makePathSymbol(const char *Path) {
    return MakeSymbol(PathSymbols, Path);
  }
};
