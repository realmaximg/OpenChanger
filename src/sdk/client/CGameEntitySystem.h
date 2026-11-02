#pragma once

#include "sdk/engine2/CGameResourceService.h"
#include "sdk/macros.h"

#include <array>
#include <cstdint>

#define PtIdentityChunks                                                       \
  "48 8B 3D ? ? ? ? 48 8D 43 * 45 33 FF 48 89 1D ? ? ? ? 48 85 DB 49 0F 44 "   \
  "C7 48 89 05"

struct CEntityIdentity;
class CEntityInstance;

class CGameEntitySystem {
public:
  SDK_CLASS(CGameEntitySystem);

  static constexpr std::uint32_t IndexMask = 0x7fff;
  static constexpr std::uint32_t ChunkSize = 0x200;
  static constexpr std::uint32_t InvalidHandle = 0xffffffff;
  static constexpr std::uint32_t UnsetHandle = 0xfffffffe;

  using Chunks = std::array<CEntityIdentity *, (IndexMask + 1) / ChunkSize>;

  PATTERN_FIELD(Client, Chunks, IdentityChunks, PtIdentityChunks);

  static CGameEntitySystem *get() {
    return CGameResourceService::get()->EntitySystem();
  }

  static std::uint32_t indexOf(std::uint64_t Handle) {
    return static_cast<std::uint32_t>(Handle) & IndexMask;
  }

  CEntityIdentity *getIdentity(std::uint32_t Index);
  CEntityInstance *getEntity(std::uint32_t Handle);
  CEntityInstance *getEntityByIndex(std::uint32_t Index);
};
