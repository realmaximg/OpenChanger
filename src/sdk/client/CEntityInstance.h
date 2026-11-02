#pragma once

#include "game/find.h"
#include "game/module.h"
#include "game/pattern.h"
#include "sdk/client/CEntityIdentity.h"
#include "sdk/client/CGameEntitySystem.h"
#include "sdk/macros.h"
#include "sdk/schemasystem/CSchemaSystem.h"
#include "sdk/schemasystem/SchemaClassInfoData_t.h"

#include <bit>
#include <cstdint>
#include <optional>
#include <string_view>

#define PtSchemaDynamicBinding                                                 \
  "40 53 48 83 EC 20 48 8B DA 48 8D 15 * * * * 48 8B CB E8 ? ? ? ? 48 8B C3 "  \
  "48 83 C4 20 5B C3"

class CEntityInstance {
  static std::optional<std::uint32_t> findSchemaDynamicBinding() {
    const SchemaClassInfoData_t *const Class =
        CSchemaSystem::get()->findDeclaredClass(oc::GameModule::Client,
                                                "C_CSPlayerPawn");
    if (Class == nullptr) {
      return std::nullopt;
    }
    return oc::findSlot(
        oc::GameModule::Client, "C_CSPlayerPawn", [Class](auto Function) {
          const auto Binding = oc::patternTargetAt(
              oc::GameModule::Client, Function, PtSchemaDynamicBinding);
          return Binding &&
                 std::bit_cast<SchemaClassInfoData_t *>(*Binding)->m_pSelf ==
                     Class;
        });
  }

public:
  SDK_CLASS(CEntityInstance);

  SCHEMA_FIELD(CEntityIdentity *, m_pEntity);
  VIRTUAL_FIND(SchemaClassInfoData_t **(SchemaClassInfoData_t **),
               Schema_DynamicBinding, findSchemaDynamicBinding);

  SchemaClassInfoData_t *getSchemaClass() {
    SchemaClassInfoData_t *Class = nullptr;
    Schema_DynamicBinding(&Class);
    return Class;
  }

  std::string_view getSchemaClassName() {
    const SchemaClassInfoData_t *const Class = getSchemaClass();
    return Class == nullptr ? std::string_view{} : Class->m_pszName;
  }

  template <class T> T *as() {
    for (const SchemaClassInfoData_t *Class = getSchemaClass();
         Class != nullptr; Class = Class->getBaseClass()) {
      if (Class->m_pszName == T::ClassName) {
        return static_cast<T *>(this);
      }
    }
    return nullptr;
  }

  std::uint32_t getHandle() {
    const CEntityIdentity *const Identity = m_pEntity();
    return Identity == nullptr ? CGameEntitySystem::InvalidHandle
                               : Identity->m_EHandle;
  }
};
