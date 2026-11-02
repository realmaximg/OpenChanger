#pragma once

#include "game/find.h"
#include "game/module.h"
#include "sdk/macros.h"

#include <cstdint>
#include <optional>

#define PtGetSoundEventHash                                                    \
  "48 8B C2 48 85 D2 74 ? 80 3A 00 74 ? 48 C7 C2 FF FF FF FF 48 FF C2 80 3C "  \
  "10 00 75 ? 41 B8 32 43 52 53 48 8B C8 E9 * * * *"

class ISoundEventManager {
  static std::optional<std::uint32_t> findGetSoundEventHash() {
    return oc::findSlotMatching(oc::GameModule::SoundSystem,
                                "CSoundEventManager", PtGetSoundEventHash);
  }

public:
  SDK_CLASS(ISoundEventManager);

  VIRTUAL_FIND(std::uint32_t(const char *, bool), GetSoundEventHash,
               findGetSoundEventHash);
};
