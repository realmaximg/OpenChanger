#pragma once

#include "sdk/client/CBaseAnimGraph.h"
#include "sdk/client/C_AttributeContainer.h"
#include "sdk/client/C_EconItemView.h"
#include "sdk/macros.h"

#include <cstdint>

class C_EconEntity : public CBaseAnimGraph {
public:
  SDK_CLASS(C_EconEntity);

  SCHEMA_FIELD(C_AttributeContainer, m_AttributeManager);
  SCHEMA_FIELD(std::uint32_t, m_OriginalOwnerXuidLow);
  SCHEMA_FIELD(std::uint32_t, m_OriginalOwnerXuidHigh);

  C_EconItemView &getItem() { return m_AttributeManager().m_Item(); }

  std::uint64_t getOriginalOwner() {
    return (std::uint64_t{m_OriginalOwnerXuidHigh()} << 32U) |
           m_OriginalOwnerXuidLow();
  }
};
