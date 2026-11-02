#pragma once

#include "sdk/macros.h"

#include <array>
#include <cstddef>
#include <cstdint>

enum ResponseType_t : std::uint8_t {
  RESPONSE_NONE,
  RESPONSE_SPEAK,
  RESPONSE_SENTENCE,
  RESPONSE_SCENE,
  RESPONSE_RESPONSE,
  RESPONSE_PRINT,
};

struct ParserResponse {
  static constexpr std::uint8_t TypeMask = 0x3f;

  PAD(0x3f);
  std::uint8_t typeBits;
  const char *value;
  PAD(0x8);

  [[nodiscard]] ResponseType_t type() const {
    return static_cast<ResponseType_t>(typeBits & TypeMask);
  }
};

static_assert(sizeof(ParserResponse) == 0x50);
static_assert(offsetof(ParserResponse, typeBits) == 0x3f);
static_assert(offsetof(ParserResponse, value) == 0x40);
