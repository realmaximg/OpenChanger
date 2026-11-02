#pragma once

#include "sdk/macros.h"
#include "sdk/tier0/tier0.h"

#include <array>
#include <cstdint>
#include <string_view>

class CUtlBuffer {
public:
  SDK_CLASS(CUtlBuffer);

  SDK_EXPORT_FUNCTION(Tier0,
                      void(CUtlBuffer *, std::int32_t, std::int32_t,
                           std::int32_t),
                      Construct, "??0CUtlBuffer@@QEAA@HHW4BufferFlags_t@0@@Z");
  SDK_EXPORT_FUNCTION(Tier0, void(CUtlBuffer *, bool, bool), SetBufferType,
                      "?SetBufferType@CUtlBuffer@@QEAAX_N0@Z");
  SDK_EXPORT_FUNCTION(Tier0, void(CUtlBuffer *, const void *, std::int32_t),
                      Put, "?Put@CUtlBuffer@@QEAAXPEBXH@Z");
  SDK_EXPORT_FUNCTION(Tier0, void(CUtlBuffer *), AddNullTermination,
                      "?AddNullTermination@CUtlBuffer@@IEAAXXZ");

  static CUtlBuffer *createText(std::string_view Text) {
    auto *const Buffer = static_cast<CUtlBuffer *>(
        tier0::MemAlloc_AllocFunc(sizeof(CUtlBuffer)));
    Construct(Buffer, 0, 0, 0);
    SetBufferType(Buffer, true, true);
    Put(Buffer, Text.data(), static_cast<std::int32_t>(Text.size()));
    AddNullTermination(Buffer);
    return Buffer;
  }

  PAD(0x40);
};

static_assert(sizeof(CUtlBuffer) == 0x40);
