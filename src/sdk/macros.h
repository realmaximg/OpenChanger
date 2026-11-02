#pragma once

#include "game/link.h"
#include "game/module.h"

#include <array>
#include <bit>
#include <cstdint>
#include <string_view>

namespace oc {

template <class T> T &fieldAt(void *Object, std::uint32_t Offset) {
  return *std::bit_cast<T *>(std::bit_cast<std::uintptr_t>(Object) + Offset);
}

template <class T>
T &elementAt(void *Array, std::int32_t Index, std::uint32_t Size) {
  return fieldAt<T>(Array, static_cast<std::uint32_t>(Index) * Size);
}

template <class Signature> struct Function;

template <class R, class... P> struct Function<R(P...)> {
  static R call(void *Address, P... Args) {
    return reinterpret_cast<R (*)(P...)>(Address)(Args...);
  }
};

template <class Signature> struct Virtual;

template <class R, class... P> struct Virtual<R(P...)> {
  static R call(void *Object, std::uint32_t Offset, P... Args) {
    const auto Table = *static_cast<std::uintptr_t *>(Object);
    auto *const Function = *std::bit_cast<R (**)(void *, P...)>(Table + Offset);
    return Function(Object, Args...);
  }
};

} // namespace oc

#define SDK_JOIN(A, B) A##B
#define SDK_PAD_NAME(Counter) SDK_JOIN(Pad, Counter)

#define PAD(Size) std::array<char, Size> SDK_PAD_NAME(__COUNTER__)

#define SDK_CLASS(Name) static constexpr std::string_view ClassName = #Name

#define SDK_FIELD(Type, Name, Offset)                                          \
  decltype(auto) Name() { return oc::fieldAt<Type>(this, Offset); }

#define SDK_VIRTUAL(Signature, Name, Offset)                                   \
  template <class... A> decltype(auto) Name(A... Args) {                       \
    return oc::Virtual<Signature>::call(this, Offset, Args...);                \
  }

#define SDK_FUNCTION(Signature, Name, Address)                                 \
  template <class... A> static decltype(auto) Name(A... Args) {                \
    return oc::Function<Signature>::call(Address, Args...);                    \
  }

#define SDK_GLOBAL(Type, Name, Address)                                        \
  static decltype(auto) Name() { return *static_cast<Type *>(Address); }

#define SDK_INTERFACE(Module, Class, Version)                                  \
  SDK_CLASS(Class);                                                            \
  static inline oc::InterfaceLink Class##Link{oc::GameModule::Module,          \
                                              ClassName, Version};             \
  static auto *get() { return static_cast<Class *>(Class##Link.address()); }

#define SCHEMA_FIELD(Type, Name)                                               \
  static inline oc::SchemaLink Name##Link{ClassName, #Name};                   \
  SDK_FIELD(Type, Name, Name##Link.offset())

#define SDK_PATTERN_LINK(Module, Name, Pattern)                                \
  static inline oc::PatternLink Name##Link{oc::GameModule::Module, ClassName,  \
                                           #Name, Pattern};

#define PATTERN_FIELD(Module, Type, Name, Pattern)                             \
  SDK_PATTERN_LINK(Module, Name, Pattern)                                      \
  SDK_FIELD(Type, Name, Name##Link.value())

#define PATTERN_VIRTUAL(Module, Signature, Name, Pattern)                      \
  SDK_PATTERN_LINK(Module, Name, Pattern)                                      \
  SDK_VIRTUAL(Signature, Name, Name##Link.value())

#define VIRTUAL_INDEX(Signature, Name, Index)                                  \
  SDK_VIRTUAL(Signature, Name,                                                 \
              static_cast<std::uint32_t>((Index) * sizeof(void *)))

#define SDK_ADDRESS_LINK(Module, Name, Pattern)                                \
  static inline oc::AddressLink Name##Link{oc::GameModule::Module, ClassName,  \
                                           #Name, Pattern};

#define PATTERN_FUNCTION(Module, Signature, Name, Pattern)                     \
  SDK_ADDRESS_LINK(Module, Name, Pattern)                                      \
  SDK_FUNCTION(Signature, Name, Name##Link.address())

#define PATTERN_GLOBAL(Module, Type, Name, Pattern)                            \
  SDK_ADDRESS_LINK(Module, Name, Pattern)                                      \
  SDK_GLOBAL(Type, Name, Name##Link.address())

#define SDK_GAME_SYSTEM(Class, SystemName)                                     \
  SDK_CLASS(Class);                                                            \
  static inline oc::GameSystemLink Class##Link{ClassName, SystemName,          \
                                               &CBaseGameSystemFactory::find}; \
  static auto *get() { return static_cast<Class *>(Class##Link.address()); }

#define SDK_EXPORT(Module, Class, Symbol)                                      \
  SDK_CLASS(Class);                                                            \
  static inline oc::ExportLink Class##Link{oc::GameModule::Module, ClassName,  \
                                           Symbol, Symbol};                    \
  static auto *get() { return *static_cast<Class **>(Class##Link.address()); }

#define SDK_EXPORT_FUNCTION(Module, Signature, Name, Decorated)                \
  static inline oc::ExportLink Name##Link{oc::GameModule::Module, ClassName,   \
                                          #Name, Decorated};                   \
  SDK_FUNCTION(Signature, Name, Name##Link.address())

#define SDK_FIND_LINK(Value, Name, Finder)                                     \
  static inline oc::FindLink<Value> Name##Link{ClassName, #Name, Finder};

#define FIELD_FIND(Type, Name, Finder)                                         \
  SDK_FIND_LINK(std::uint32_t, Name, Finder)                                   \
  SDK_FIELD(Type, Name, Name##Link.value())

#define VIRTUAL_FIND(Signature, Name, Finder)                                  \
  SDK_FIND_LINK(std::uint32_t, Name, Finder)                                   \
  SDK_VIRTUAL(Signature, Name, Name##Link.value())

#define FUNCTION_FIND(Signature, Name, Finder)                                 \
  SDK_FIND_LINK(std::uintptr_t, Name, Finder)                                  \
  SDK_FUNCTION(Signature, Name, std::bit_cast<void *>(Name##Link.value()))

#define GLOBAL_FIND(Type, Name, Finder)                                        \
  SDK_FIND_LINK(std::uintptr_t, Name, Finder)                                  \
  SDK_GLOBAL(Type, Name, std::bit_cast<void *>(Name##Link.value()))
