#pragma once

#include "sdk/client/CAttributeManager.h"
#include "sdk/client/C_EconItemView.h"
#include "sdk/macros.h"

class C_AttributeContainer : public CAttributeManager {
public:
  SDK_CLASS(C_AttributeContainer);

  SCHEMA_FIELD(C_EconItemView, m_Item);
};
