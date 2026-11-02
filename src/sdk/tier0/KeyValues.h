#pragma once

#include "sdk/macros.h"

class KeyValues {
public:
  SDK_CLASS(KeyValues);

  SDK_EXPORT_FUNCTION(Tier0, const KeyValues *(const KeyValues *, const char *),
                      FindKey, "?FindKey@KeyValues@@QEBAPEBV1@PEBD@Z");
};
