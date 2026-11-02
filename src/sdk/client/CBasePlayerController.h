#pragma once

#include "sdk/client/C_BaseEntity.h"
#include "sdk/macros.h"

#include <cstdint>

class CBasePlayerController : public C_BaseEntity {
public:
  SDK_CLASS(CBasePlayerController);

  SCHEMA_FIELD(std::uint64_t, m_steamID);
};
