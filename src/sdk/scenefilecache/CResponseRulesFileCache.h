#pragma once

#include "sdk/macros.h"
#include "sdk/scenefilecache/CResponseSystem.h"

#include <optional>
#include <utility>

#define PtLoadResponseSystem                                                   \
  "48 8B 0D ? ? ? ? 48 8B D6 48 89 9C 24 ? ? ? ? 48 8B 01 FF 50 * 48 8B D8 "   \
  "48 85 C0 75"

class CResponseRulesFileCache {
public:
  SDK_INTERFACE(SceneFileCache, CResponseRulesFileCache,
                "ResponseRulesCache001");

  PATTERN_VIRTUAL(Client, CResponseSystem *(const char *), LoadResponseSystem,
                  PtLoadResponseSystem);

  template <class Read> static auto readRules(Read Use) {
    using Result = decltype(Use(std::declval<CResponseSystem &>()));
    CResponseSystem *const System =
        get()->LoadResponseSystem(CResponseSystem::RulesPath);
    if (System == nullptr) {
      return std::optional<Result>{};
    }
    std::optional<Result> Value = Use(*System);
    System->RefCounted.Release();
    return Value;
  }
};
