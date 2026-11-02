#pragma once

#include "sdk/client/CEntityInstance.h"
#include "sdk/client/client.h"
#include "sdk/macros.h"
#include "sdk/tier1/CUtlStringToken.h"

#include <cstdint>

#define PtEmitSound                                                            \
  "48 8B C4 53 55 57 48 81 EC 90 00 00 00 0F 29 70 D8 0F 28 F3 49 8B F8 "      \
  "48 8B DA 48 8B E9 4D 85 C0 0F 84"

class C_BaseEntity : public CEntityInstance {
public:
  SDK_CLASS(C_BaseEntity);

  SCHEMA_FIELD(CUtlStringToken, m_nSubclassID);
  SCHEMA_FIELD(std::uint8_t, m_iTeamNum);
  PATTERN_FUNCTION(Client,
                   SoundEventStart *(C_BaseEntity *, SoundEventStart *,
                                     const char *, float, float *),
                   EmitSound, PtEmitSound);

  void setSubclass(const char *Name) {
    CUtlStringToken::Make(&m_nSubclassID(), Name);
  }
};
