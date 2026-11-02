#include "game/find.h"

#include "game/module.h"
#include "game/pattern.h"

#include <libhat/scanner.hpp>
#include <libhat/signature.hpp>

#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <span>
#include <string>
#include <string_view>

namespace oc {

namespace {

bool contains(std::span<const std::byte> Section, std::uintptr_t Address) {
  const auto Begin = std::bit_cast<std::uintptr_t>(Section.data());
  return Address >= Begin && Address - Begin < Section.size();
}

std::uintptr_t find(std::span<const std::byte> Section,
                    hat::signature_view Signature) {
  return std::bit_cast<std::uintptr_t>(
      hat::find_pattern(Section, Signature).get());
}

} // namespace

std::span<const std::uintptr_t> vtableOf(GameModule Module,
                                         std::string_view Class) {
  const std::string Name = ".?AV" + std::string(Class) + "@@" + '\0';
  const std::uintptr_t TypeName =
      find(sectionOf(Module, ".data"), hat::string_to_signature(Name).value());
  if (TypeName == 0) {
    return {};
  }
  const auto TypeDescriptor = static_cast<std::uint32_t>(
      TypeName - (2 * sizeof(void *)) - baseOf(Module));
  const auto Rdata = sectionOf(Module, ".rdata");
  const std::array<std::uint32_t, 4> Header{1, 0, 0, TypeDescriptor};
  const std::uintptr_t Locator = find(Rdata, hat::object_to_signature(Header));
  if (Locator == 0) {
    return {};
  }
  const std::uintptr_t Meta = find(Rdata, hat::object_to_signature(Locator));
  if (Meta == 0) {
    return {};
  }
  const auto *const First =
      std::bit_cast<const std::uintptr_t *>(Meta + sizeof(void *));
  const auto Text = sectionOf(Module, ".text");
  std::size_t Size = 0;
  while (contains(Rdata, std::bit_cast<std::uintptr_t>(First + Size)) &&
         contains(Text, First[Size])) {
    ++Size;
  }
  return {First, Size};
}

// NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
std::optional<std::uint32_t> findSlotMatching(GameModule Module,
                                              std::string_view Class,
                                              std::string_view Pattern) {
  return findSlot(Module, Class, [Module, Pattern](std::uintptr_t Function) {
    return patternValueAt(Module, Function, Pattern).has_value();
  });
}

// NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
std::optional<std::uint32_t> findSlotNamed(GameModule Module,
                                           std::string_view Class,
                                           std::string_view Pattern,
                                           std::string_view Name) {
  return findSlot(
      Module, Class, [Module, Pattern, Name](std::uintptr_t Function) {
        const auto Text = patternTargetAt(Module, Function, Pattern);
        return Text &&
               std::string_view(std::bit_cast<const char *>(*Text)) == Name;
      });
}

} // namespace oc
