#pragma once

#include "sdk/macros.h"

#include <array>
#include <cstddef>
#include <cstdint>

struct CBufferString {
  SDK_CLASS(CBufferString);

  SDK_EXPORT_FUNCTION(Tier0, void(CBufferString *, std::int32_t), Purge,
                      "?Purge@CBufferString@@QEAAXH@Z");
  SDK_EXPORT_FUNCTION(
      Tier0, bool(const CBufferString *, const char *),
      EndsWith_FastCaseInsensitive,
      "?EndsWith_FastCaseInsensitive@CBufferString@@QEBA_NPEBD@Z");

  static constexpr std::uint32_t STACK_ALLOCATED_MARKER = 0x40000000;
  static constexpr std::uint32_t ALLOW_HEAP_ALLOCATION = 0x80000000;

  std::int32_t m_nLength;
  std::uint32_t m_nAllocatedSize;
};

static_assert(sizeof(CBufferString) == 0x8);
static_assert(offsetof(CBufferString, m_nAllocatedSize) == 0x4);

template <std::uint32_t Size> struct CBufferStringN {
  static constexpr std::uint32_t DATA_SIZE = Size - sizeof(CBufferString);

  CBufferStringN() = default;
  ~CBufferStringN() { CBufferString::Purge(&m_String, 0); }
  CBufferStringN(const CBufferStringN &) = delete;
  CBufferStringN &operator=(const CBufferStringN &) = delete;

  CBufferString m_String{
      .m_nLength = 0,
      .m_nAllocatedSize = DATA_SIZE | CBufferString::STACK_ALLOCATED_MARKER |
                          CBufferString::ALLOW_HEAP_ALLOCATION,
  };
  std::array<char, DATA_SIZE> m_FixedData{};
};

static_assert(sizeof(CBufferStringN<0x10>) == 0x10);
