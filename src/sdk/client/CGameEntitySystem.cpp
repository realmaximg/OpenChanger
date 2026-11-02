#include "sdk/client/CGameEntitySystem.h"

#include "sdk/client/CEntityIdentity.h"

#include <cstdint>

CEntityIdentity *CGameEntitySystem::getIdentity(std::uint32_t Index) {
  CEntityIdentity *const Chunk =
      IdentityChunks()[(Index & IndexMask) / ChunkSize];
  return Chunk == nullptr ? nullptr : &Chunk[Index % ChunkSize];
}

CEntityInstance *CGameEntitySystem::getEntity(std::uint32_t Handle) {
  if (Handle == InvalidHandle || Handle == UnsetHandle) {
    return nullptr;
  }
  const CEntityIdentity *const Identity = getIdentity(Handle);
  if (Identity == nullptr || Identity->m_EHandle != Handle) {
    return nullptr;
  }
  return Identity->m_pInstance;
}

CEntityInstance *CGameEntitySystem::getEntityByIndex(std::uint32_t Index) {
  const CEntityIdentity *const Identity = getIdentity(Index);
  if (Identity == nullptr || (Identity->m_EHandle & IndexMask) != Index) {
    return nullptr;
  }
  return Identity->m_pInstance;
}
