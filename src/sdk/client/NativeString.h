#pragma once

#include "sdk/tier0/IMemAlloc.h"

#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <string_view>

struct NativeString {
  std::array<char, 16> m_Bx{};
  std::size_t m_Mysize = 0;
  std::size_t m_Myres = 15;

  [[nodiscard]] bool isLarge() const { return m_Myres >= m_Bx.size(); }

  [[nodiscard]] const char *data() const {
    if (!isLarge()) {
      return m_Bx.data();
    }
    std::uintptr_t Address = 0;
    std::memcpy(&Address, m_Bx.data(), sizeof(Address));
    return std::bit_cast<const char *>(Address);
  }

  NativeString() = default;
  NativeString(const NativeString &) = delete;
  NativeString &operator=(const NativeString &) = delete;

  ~NativeString() {
    if (!isLarge()) {
      return;
    }
    auto Block = std::bit_cast<std::uintptr_t>(data());
    if (m_Myres + 1 >= 0x1000) {
      Block = *std::bit_cast<const std::uintptr_t *>(Block - sizeof(Block));
    }
    IMemAlloc::get()->Free(std::bit_cast<void *>(Block));
  }

  [[nodiscard]] std::string_view view() const { return {data(), m_Mysize}; }
};

static_assert(sizeof(NativeString) == 0x20);
static_assert(offsetof(NativeString, m_Mysize) == 0x10);
static_assert(offsetof(NativeString, m_Myres) == 0x18);
