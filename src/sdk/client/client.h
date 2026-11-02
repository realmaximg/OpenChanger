#pragma once

#include "sdk/macros.h"

#include <array>
#include <cstdint>

#define PtDoStartSoundEvent                                                    \
  "48 8B C4 55 53 56 57 41 54 48 8D A8 ? ? ? ? 48 81 EC ? ? ? ? 44 8B A5 ? ? " \
  "? ? 33 DB 4C ? ? ? 48 8B F2 89 ? 41 8B F9 C7 42 04 ? ? ? ? 89 ? ? 4C ? ? "  \
  "? 4C ? ? ? 33 C0"
#define PtEconRarity_CombinedItemAndPaintRarity                                \
  "45 33 C0 8D 42 FF 83 FA 07 41 0F 94 C0 41 83 C0 06 03 C1 79 03 33 C0 C3"
#define PtMainMenu                                                             \
  "48 83 EC 28 48 8B 05 * * * * 48 8D 15 ? ? ? ? 48 8B 48 08 48 8B 01 FF 90 "  \
  "78 01 00 00"

class CPanel2D;

struct alignas(4) SoundEventStart {
  PAD(0x14);
};

static_assert(sizeof(SoundEventStart) == 0x14);

struct client {
  SDK_CLASS(client);

  PATTERN_FUNCTION(Client,
                   SoundEventStart *(void *, SoundEventStart *, std::uint64_t,
                                     std::uint64_t, std::uint64_t,
                                     std::uint64_t, std::uint64_t,
                                     std::uint64_t, std::uint64_t, void *),
                   DoStartSoundEvent, PtDoStartSoundEvent);
  PATTERN_FUNCTION(Client, std::int32_t(std::int32_t, std::int32_t),
                   EconRarity_CombinedItemAndPaintRarity,
                   PtEconRarity_CombinedItemAndPaintRarity);
  PATTERN_GLOBAL(Client, CPanel2D *, MainMenu, PtMainMenu);
};
