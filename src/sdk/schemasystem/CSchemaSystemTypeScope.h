#pragma once

#include "sdk/macros.h"
#include "sdk/schemasystem/SchemaClassInfoData_t.h"

#define PtFindDeclaredClass                                                    \
  "40 53 55 57 48 83 EC 20 49 8B D8 48 8B FA E8 ? ? ? ? 48 8B 0D ? ? ? ? 45 "  \
  "33 C0 48 8B D0 4C 8B 09 41 FF 51 ? 4C 8B C3 48 8D 54 24 ? 48 8B C8 4C 8B "  \
  "08 41 FF 51 *"

class CSchemaSystemTypeScope {
public:
  SDK_CLASS(CSchemaSystemTypeScope);

  PATTERN_VIRTUAL(Client,
                  SchemaClassInfoData_t **(SchemaClassInfoData_t **,
                                           const char *),
                  FindDeclaredClass, PtFindDeclaredClass);
};
