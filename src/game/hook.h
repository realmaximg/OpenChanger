#pragma once

#include "game/link.h"

#include <safetyhook.hpp>

#include <utility>

namespace oc {

class Hook {
public:
  Hook(const Hook &) = delete;
  Hook &operator=(const Hook &) = delete;

protected:
  class CallGuard {
  public:
    CallGuard() noexcept;
    ~CallGuard();
    CallGuard(const CallGuard &) = delete;
    CallGuard &operator=(const CallGuard &) = delete;
  };

  Hook() noexcept;
  ~Hook() = default;

private:
  friend bool installHooks();
  friend void removeHooks();

  virtual bool install() = 0;
  virtual void remove() = 0;
};

bool installHooks();
void removeHooks();

template <auto Detour> class InlineHook;

template <class R, class... A, R (*Detour)(A...)>
class InlineHook<Detour> final : public Hook {
public:
  explicit InlineHook(const AddressLink &Target) noexcept : Target(&Target) {}

  [[nodiscard]] R original(A... Args) const {
    return Installed.original<R (*)(A...)>()(Args...);
  }

private:
  static R call(A... Args) {
    const CallGuard Guard;
    return Detour(Args...);
  }

  bool install() override {
    auto Created = safetyhook::InlineHook::create(Target->address(), &call);
    if (!Created) {
      return false;
    }
    Installed = std::move(*Created);
    return true;
  }

  void remove() override {
    [[maybe_unused]] const auto Disabled = Installed.disable();
  }

  const AddressLink *Target;
  safetyhook::InlineHook Installed;
};

template <auto Detour> class VirtualHook;

template <class R, class Self, class... A, R (*Detour)(Self *, A...)>
class VirtualHook<Detour> final : public Hook {
public:
  VirtualHook(Self *(*Object)(), const PatternLink &Slot) noexcept
      : Object(Object), Slot(&Slot) {}

  [[nodiscard]] R original(Self *This, A... Args) const {
    return Replaced.original<R (*)(Self *, A...)>()(This, Args...);
  }

private:
  static R call(Self *This, A... Args) {
    const CallGuard Guard;
    return Detour(This, Args...);
  }

  bool install() override {
    Self *const Instance = Object();
    if (Instance == nullptr) {
      return false;
    }
    auto Table = safetyhook::VmtHook::create(Instance);
    if (!Table) {
      return false;
    }
    auto Method = Table->hook_method(Slot->value() / sizeof(void *), &call);
    if (!Method) {
      return false;
    }
    Installed = std::move(*Table);
    Replaced = std::move(*Method);
    return true;
  }

  void remove() override { Installed.reset(); }

  Self *(*Object)();
  const PatternLink *Slot;
  safetyhook::VmtHook Installed;
  safetyhook::VmHook Replaced;
};

} // namespace oc
