#pragma once

#include "sdk/macros.h"
#include "sdk/scenefilecache/ParserResponse.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <span>

struct ResponseGroup {
  std::int16_t m_Size;
  PAD(0x6);
  ParserResponse *m_pElements;

  [[nodiscard]] std::span<const ParserResponse> group() const {
    return {m_pElements, static_cast<std::size_t>(m_Size)};
  }
};

static_assert(offsetof(ResponseGroup, m_Size) == 0x0);
static_assert(offsetof(ResponseGroup, m_pElements) == 0x8);
