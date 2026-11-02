#include "game/link.h"

#include "core/log.h"
#include "game/module.h"
#include "game/pattern.h"
#include "sdk/schemasystem/CSchemaSystem.h"
#include "sdk/schemasystem/SchemaClassFieldData_t.h"
#include "sdk/schemasystem/SchemaClassInfoData_t.h"

#include <bit>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace oc {

namespace {

std::vector<Link *> &registry() {
  static std::vector<Link *> Links;
  return Links;
}

std::optional<std::uintptr_t> nonZero(std::uintptr_t Value) {
  if (Value == 0) {
    return std::nullopt;
  }
  return Value;
}

} // namespace

Link::Link(Kind Order, std::string_view ClassName,
           std::string_view Name) noexcept
    : Order(Order), ClassName(ClassName), Name(Name) {
  registry().push_back(this);
}

std::optional<std::uintptr_t> ExportLink::resolve() const {
  return nonZero(exportOf(Module, Symbol));
}

std::optional<std::uintptr_t> InterfaceLink::resolve() const {
  return nonZero(std::bit_cast<std::uintptr_t>(
      createInterface(Module, std::string(name()).c_str())));
}

std::optional<std::uintptr_t> PatternLink::resolve() const {
  const auto Match = findPattern(Module, Pattern);
  if (!Match) {
    return std::nullopt;
  }
  return patternValueAt(Module, *Match, Pattern);
}

std::optional<std::uintptr_t> AddressLink::resolve() const {
  const auto Match = findPattern(Module, Pattern);
  if (!Match || !Pattern.contains('*')) {
    return Match;
  }
  return patternTargetAt(Module, *Match, Pattern);
}

std::optional<std::uintptr_t> GameSystemLink::resolve() const {
  return nonZero(std::bit_cast<std::uintptr_t>(Lookup(name())));
}

std::optional<std::uintptr_t> SchemaLink::resolve() const {
  const std::string Class(className());
  const SchemaClassInfoData_t *const Info =
      CSchemaSystem::get()->findDeclaredClass(GameModule::Client,
                                              Class.c_str());
  if (Info == nullptr) {
    return std::nullopt;
  }
  for (const SchemaClassFieldData_t &Field : Info->getFields()) {
    if (std::string_view(Field.m_pszName) == name()) {
      return Field.m_nSingleInheritanceOffset;
    }
  }
  return std::nullopt;
}

bool resolveLink(Link &Entry) {
  const auto Value = Entry.resolve();
  if (!Value) {
    OC_LOG("link", "missing {}::{}", Entry.ClassName, Entry.Name);
    return false;
  }
  Entry.Value = *Value;
  OC_LOG("link", "{}::{} = {:#x}", Entry.ClassName, Entry.Name, *Value);
  return true;
}

std::size_t resolveLinks() {
  std::size_t Missing = 0;
  for (const Link::Kind Order : {
           Link::Kind::Export,
           Link::Kind::Interface,
           Link::Kind::Pattern,
           Link::Kind::GameSystem,
           Link::Kind::Schema,
           Link::Kind::Find,
       }) {
    for (Link *const Entry : registry()) {
      if (Entry->Order == Order && !resolveLink(*Entry)) {
        ++Missing;
      }
    }
    if (Missing != 0) {
      break;
    }
  }
  return Missing;
}

} // namespace oc
