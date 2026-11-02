#pragma once

#include "game/find.h"
#include "game/module.h"
#include "sdk/macros.h"
#include "sdk/tier0/CUtlBuffer.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <string_view>

#define PtFindInsertionPosition                                                \
  "49 8D 8D A0 00 00 00 0F 11 45 ? 66 89 45 ? 0F 10 45 ? 0F 11 45 ? E8 * * * " \
  "* 83 7C 24 ? FF 74 ? FF 15"
#define PtInsertAt                                                             \
  "49 8D 8D A8 00 00 00 8B 44 24 ? 4C 8D 45 ? 48 8D 54 24 ? F2 0F 11 44 24 ? " \
  "89 44 24 ? E8 * * * *"
#define PtDiscardEdits                                                         \
  "40 55 41 54 41 57 48 83 EC ? 8B 91 * * * * 45 33 E4 4C 8B F9 41 8B EC F7 "  \
  "C2 FF FF FF 7F 0F 86"

struct FileOverride {
  std::uint16_t m_Symbol;
  PAD(0x6);
  std::array<std::uint8_t, 20> m_Sha1;
  PAD(0x4);
  CUtlBuffer *m_pBuffer;
};

static_assert(sizeof(FileOverride) == 0x28);
static_assert(offsetof(FileOverride, m_Sha1) == 0x8);
static_assert(offsetof(FileOverride, m_pBuffer) == 0x20);

struct FileOverridePosition {
  PAD(0x4);
  std::int32_t m_Existing;
  PAD(0x4);
};

static_assert(sizeof(FileOverridePosition) == 0xC);
static_assert(offsetof(FileOverridePosition, m_Existing) == 0x4);

class CLayoutManager {
  static constexpr std::uint32_t FileOverrides = 0xA0;
  static constexpr std::uint32_t FileOverrideTree = FileOverrides + 0x8;

  static std::optional<std::uint32_t> findDiscardEdits() {
    return oc::findSlotMatching(oc::GameModule::Panorama,
                                "CLayoutManager@panorama", PtDiscardEdits);
  }

public:
  SDK_CLASS(CLayoutManager);

  PATTERN_FUNCTION(Panorama,
                   void(void *, FileOverridePosition *, const FileOverride *),
                   FindInsertionPosition, PtFindInsertionPosition);
  PATTERN_FUNCTION(Panorama,
                   std::int32_t(void *, const FileOverridePosition *,
                                const FileOverride *),
                   InsertAt, PtInsertAt);
  VIRTUAL_FIND(void(), DiscardEdits, findDiscardEdits);

  bool addFileOverride(std::uint16_t Symbol, std::string_view Text) {
    FileOverride Entry{.m_Symbol = Symbol};
    FileOverridePosition Position{};
    FindInsertionPosition(&oc::fieldAt<char>(this, FileOverrides), &Position,
                          &Entry);
    if (Position.m_Existing != -1) {
      return false;
    }
    Entry.m_pBuffer = CUtlBuffer::createText(Text);
    InsertAt(&oc::fieldAt<char>(this, FileOverrideTree), &Position, &Entry);
    return true;
  }
};
