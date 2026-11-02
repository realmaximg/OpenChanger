#pragma once

#include "game/module.h"

#include <bit>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <string_view>

namespace oc {

class Link {
public:
  Link(const Link &) = delete;
  Link &operator=(const Link &) = delete;

protected:
  enum class Kind : std::uint8_t {
    Export,
    Interface,
    Pattern,
    GameSystem,
    Schema,
    Find,
  };

  Link(Kind Order, std::string_view ClassName, std::string_view Name) noexcept;
  ~Link() = default;

  [[nodiscard]] std::string_view className() const { return ClassName; }
  [[nodiscard]] std::string_view name() const { return Name; }
  [[nodiscard]] std::uintptr_t resolved() const { return Value; }

private:
  friend bool resolveLink(Link &Entry);
  friend std::size_t resolveLinks();

  [[nodiscard]] virtual std::optional<std::uintptr_t> resolve() const = 0;

  Kind Order;
  std::string_view ClassName;
  std::string_view Name;
  std::uintptr_t Value = 0;
};

class ExportLink final : public Link {
public:
  // NOLINTBEGIN(bugprone-easily-swappable-parameters)
  ExportLink(GameModule Module, std::string_view ClassName,
             std::string_view Name, std::string_view Symbol) noexcept
      : Link(Kind::Export, ClassName, Name), Module(Module), Symbol(Symbol) {}
  // NOLINTEND(bugprone-easily-swappable-parameters)

  [[nodiscard]] void *address() const {
    return std::bit_cast<void *>(resolved());
  }

private:
  [[nodiscard]] std::optional<std::uintptr_t> resolve() const override;

  GameModule Module;
  std::string_view Symbol;
};

class InterfaceLink final : public Link {
public:
  InterfaceLink(GameModule Module, std::string_view ClassName,
                std::string_view Name) noexcept
      : Link(Kind::Interface, ClassName, Name), Module(Module) {}

  [[nodiscard]] void *address() const {
    return std::bit_cast<void *>(resolved());
  }

private:
  [[nodiscard]] std::optional<std::uintptr_t> resolve() const override;

  GameModule Module;
};

class PatternLink final : public Link {
public:
  // NOLINTBEGIN(bugprone-easily-swappable-parameters)
  PatternLink(GameModule Module, std::string_view ClassName,
              std::string_view Name, std::string_view Pattern) noexcept
      : Link(Kind::Pattern, ClassName, Name), Module(Module), Pattern(Pattern) {
  }
  // NOLINTEND(bugprone-easily-swappable-parameters)

  [[nodiscard]] std::uint32_t value() const {
    return static_cast<std::uint32_t>(resolved());
  }

private:
  [[nodiscard]] std::optional<std::uintptr_t> resolve() const override;

  GameModule Module;
  std::string_view Pattern;
};

class AddressLink final : public Link {
public:
  // NOLINTBEGIN(bugprone-easily-swappable-parameters)
  AddressLink(GameModule Module, std::string_view ClassName,
              std::string_view Name, std::string_view Pattern) noexcept
      : Link(Kind::Pattern, ClassName, Name), Module(Module), Pattern(Pattern) {
  }
  // NOLINTEND(bugprone-easily-swappable-parameters)

  [[nodiscard]] void *address() const {
    return std::bit_cast<void *>(resolved());
  }

private:
  [[nodiscard]] std::optional<std::uintptr_t> resolve() const override;

  GameModule Module;
  std::string_view Pattern;
};

class GameSystemLink final : public Link {
public:
  using Find = void *(*)(std::string_view Name);

  GameSystemLink(std::string_view ClassName, std::string_view Name,
                 Find Lookup) noexcept
      : Link(Kind::GameSystem, ClassName, Name), Lookup(Lookup) {}

  [[nodiscard]] void *address() const {
    return std::bit_cast<void *>(resolved());
  }

private:
  [[nodiscard]] std::optional<std::uintptr_t> resolve() const override;

  Find Lookup;
};

class SchemaLink final : public Link {
public:
  SchemaLink(std::string_view ClassName, std::string_view Name) noexcept
      : Link(Kind::Schema, ClassName, Name) {}

  [[nodiscard]] std::uint32_t offset() const {
    return static_cast<std::uint32_t>(resolved());
  }

private:
  [[nodiscard]] std::optional<std::uintptr_t> resolve() const override;
};

template <class T> class FindLink final : public Link {
public:
  using Find = std::optional<T> (*)();

  FindLink(std::string_view ClassName, std::string_view Name,
           Find Finder) noexcept
      : Link(Kind::Find, ClassName, Name), Finder(Finder) {}

  [[nodiscard]] T value() const { return static_cast<T>(resolved()); }

private:
  [[nodiscard]] std::optional<std::uintptr_t> resolve() const override {
    return Finder().transform(
        [](T Value) { return static_cast<std::uintptr_t>(Value); });
  }

  Find Finder;
};

bool resolveLink(Link &Entry);
std::size_t resolveLinks();

} // namespace oc
