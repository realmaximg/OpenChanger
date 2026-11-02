#pragma once

#include "sdk/client/CCSGCClientSystem.h"
#include "sdk/client/CGCClient.h"
#include "sdk/client/CSharedObjectCache.h"
#include "sdk/macros.h"

#include <cstdint>
#include <string>

#define PtBCreateFromMsg                                                       \
  "48 89 5C 24 10 48 89 7C 24 18 4C 89 64 24 20 55 41 56 41 57 48 8B EC 48 "   \
  "81 EC 80 00 00 00 4C 8B FA 49 8B D9 41 8B D0 4C 8B F1 E8 ? ? ? ? 44 8B 45 " \
  "40"
#define PtBDestroyFromMsg                                                      \
  "48 89 5C 24 08 48 89 6C 24 10 48 89 74 24 18 57 48 81 EC 90 00 00 00 48 "   \
  "8B EA 49 8B F9 41 8B D0 48 8B D9 E8 ? ? ? ? 48 8B F0 48 85 C0 0F 84 38 01 " \
  "00 00"

namespace GCSDK {

class CGCClientSharedObjectCache : public CSharedObjectCache {
public:
  SDK_CLASS(CGCClientSharedObjectCache);

  PATTERN_FUNCTION(Client,
                   bool(CGCClientSharedObjectCache *, CGCClient *, std::int32_t,
                        const void *, std::uint32_t),
                   BCreateFromMsg, PtBCreateFromMsg);
  PATTERN_FUNCTION(Client,
                   bool(CGCClientSharedObjectCache *, CGCClient *, std::int32_t,
                        const void *, std::uint32_t),
                   BDestroyFromMsg, PtBDestroyFromMsg);

  bool put(std::int32_t Type, const std::string &Bytes) {
    return BCreateFromMsg(this, &CCSGCClientSystem::get()->m_GCClient(), Type,
                          Bytes.data(),
                          static_cast<std::uint32_t>(Bytes.size()));
  }

  bool destroy(std::int32_t Type, const std::string &Bytes) {
    return BDestroyFromMsg(this, &CCSGCClientSystem::get()->m_GCClient(), Type,
                           Bytes.data(),
                           static_cast<std::uint32_t>(Bytes.size()));
  }
};

} // namespace GCSDK
