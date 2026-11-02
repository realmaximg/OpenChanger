#pragma once

#include "sdk/client/CAnimGraphControllerBase.h"
#include "sdk/client/C_BaseModelEntity.h"
#include "sdk/macros.h"

class CBaseAnimGraph : public C_BaseModelEntity {
public:
  SDK_CLASS(CBaseAnimGraph);

  SCHEMA_FIELD(CAnimGraphControllerBase *, m_pMainGraphController);
};
