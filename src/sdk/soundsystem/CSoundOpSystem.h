#pragma once

#include "sdk/macros.h"
#include "sdk/soundsystem/ISoundEventManager.h"

#include <cstdint>

#define PtSoundEventManager                                                    \
  "48 8B 0D ? ? ? ? 41 8B D4 48 83 C1 * 0F 29 B4 24 ? ? ? ? 48 8B 01 FF "      \
  "50 10 48 8B 0D"

class CSoundOpSystem {
public:
  SDK_INTERFACE(SoundSystem, CSoundOpSystem, "SoundOpSystem001");

  PATTERN_FIELD(Client, ISoundEventManager, SoundEventManager,
                PtSoundEventManager);

  static std::uint32_t getSoundEventHash(const char *Name) {
    return get()->SoundEventManager().GetSoundEventHash(Name, true);
  }

  static std::uint32_t soundEventOf(std::uint64_t Packed) {
    return static_cast<std::uint32_t>(Packed);
  }

  static std::uint64_t withSoundEvent(std::uint64_t Packed, const char *Name) {
    return (Packed & ~std::uint64_t{0xffffffff}) | getSoundEventHash(Name);
  }
};
