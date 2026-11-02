#pragma once

#include "game/module.h"
#include "sdk/macros.h"
#include "sdk/schemasystem/CSchemaSystemTypeScope.h"
#include "sdk/schemasystem/SchemaClassInfoData_t.h"

#define PtGlobalTypeScope "66 3B F9 75 ? 49 8B 06 49 8B CE FF 50 *"
#define PtFindTypeScopeForModule                                               \
  "40 53 55 57 48 83 EC 20 49 8B D8 48 8B FA E8 ? ? ? ? 48 8B 0D ? ? ? ? 45 "  \
  "33 C0 48 8B D0 4C 8B 09 41 FF 51 *"

class CSchemaSystem {
public:
  SDK_INTERFACE(SchemaSystem, CSchemaSystem, "SchemaSystem_001");

  PATTERN_VIRTUAL(SchemaSystem, CSchemaSystemTypeScope *(), GlobalTypeScope,
                  PtGlobalTypeScope);
  PATTERN_VIRTUAL(Client, CSchemaSystemTypeScope *(const char *, const char **),
                  FindTypeScopeForModule, PtFindTypeScopeForModule);

  SchemaClassInfoData_t *findDeclaredClass(oc::GameModule Module,
                                           const char *Name) {
    CSchemaSystemTypeScope *const Scope =
        FindTypeScopeForModule(oc::fileName(Module), nullptr);
    if (Scope == nullptr || Scope == GlobalTypeScope()) {
      return nullptr;
    }
    SchemaClassInfoData_t *Class = nullptr;
    Scope->FindDeclaredClass(&Class, Name);
    return Class;
  }
};
