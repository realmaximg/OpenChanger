#pragma once

#include "sdk/v8/Context.h"
#include "sdk/v8/FunctionCallbackInfo.h"
#include "sdk/v8/Isolate.h"
#include "sdk/v8/Local.h"
#include "sdk/v8/Object.h"
#include "sdk/v8/Value.h"

#include <glaze/core/reflect.hpp>

#include <algorithm>
#include <array>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <format>
#include <functional>
#include <map>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

namespace oc {

class JsCall {
public:
  JsCall(const v8::FunctionCallbackInfo &Info,
         v8::Local<v8::Context> Context) noexcept;

  [[nodiscard]] v8::Local<v8::Value> argument(std::size_t Index) const;
  void setResult(v8::Local<v8::Value> Value) const;

  [[nodiscard]] static bool isNumber(v8::Local<v8::Value> Value);
  [[nodiscard]] static bool isString(v8::Local<v8::Value> Value);
  [[nodiscard]] static bool isBoolean(v8::Local<v8::Value> Value);
  [[nodiscard]] static bool isArray(v8::Local<v8::Value> Value);
  [[nodiscard]] static bool isObject(v8::Local<v8::Value> Value);
  [[nodiscard]] static bool isFunction(v8::Local<v8::Value> Value);
  [[nodiscard]] std::int64_t integer(v8::Local<v8::Value> Value) const;
  [[nodiscard]] double number(v8::Local<v8::Value> Value) const;
  [[nodiscard]] std::string string(v8::Local<v8::Value> Value) const;
  [[nodiscard]] bool boolean(v8::Local<v8::Value> Value) const;
  [[nodiscard]] static std::uint32_t length(v8::Local<v8::Value> Array);
  [[nodiscard]] v8::Local<v8::Value> get(v8::Local<v8::Value> Object,
                                         v8::Local<v8::Value> Key) const;

  [[nodiscard]] v8::Local<v8::Value> value(double Value) const;
  [[nodiscard]] v8::Local<v8::Value> value(std::string_view Value) const;
  [[nodiscard]] v8::Local<v8::Value> truth(bool Value) const;
  [[nodiscard]] v8::Local<v8::Object> array() const;
  [[nodiscard]] v8::Local<v8::Object> object() const;
  void set(v8::Local<v8::Object> Target, v8::Local<v8::Value> Key,
           v8::Local<v8::Value> Item) const;

private:
  const v8::FunctionCallbackInfo *Info;
  v8::Local<v8::Context> Context;
};

using JsDeclarations = std::map<std::string, std::string, std::less<>>;

template <class T> struct JsType;

template <class T> struct JsEnum;

template <class T> constexpr bool IsOptional = false;
template <class T> constexpr bool IsOptional<std::optional<T>> = true;

template <> struct JsType<bool> {
  static std::string declare(JsDeclarations & /*Types*/) { return "boolean"; }
  static bool read(const JsCall &Call, v8::Local<v8::Value> Value, bool &Into) {
    if (!JsCall::isBoolean(Value)) {
      return false;
    }
    Into = Call.boolean(Value);
    return true;
  }
  static v8::Local<v8::Value> write(const JsCall &Call, bool Value) {
    return Call.truth(Value);
  }
};

template <class T>
  requires std::integral<T> || std::floating_point<T>
struct JsType<T> {
  static std::string declare(JsDeclarations & /*Types*/) { return "number"; }
  static bool read(const JsCall &Call, v8::Local<v8::Value> Value, T &Into) {
    if (!JsCall::isNumber(Value)) {
      return false;
    }
    if constexpr (std::integral<T>) {
      Into = static_cast<T>(Call.integer(Value));
    } else {
      Into = static_cast<T>(Call.number(Value));
    }
    return true;
  }
  static v8::Local<v8::Value> write(const JsCall &Call, T Value) {
    return Call.value(static_cast<double>(Value));
  }
};

template <> struct JsType<std::string> {
  static std::string declare(JsDeclarations & /*Types*/) { return "string"; }
  static bool read(const JsCall &Call, v8::Local<v8::Value> Value,
                   std::string &Into) {
    if (!JsCall::isString(Value)) {
      return false;
    }
    Into = Call.string(Value);
    return true;
  }
  static v8::Local<v8::Value> write(const JsCall &Call,
                                    const std::string &Value) {
    return Call.value(std::string_view{Value});
  }
};

template <class T> struct JsType<std::vector<T>> {
  static std::string declare(JsDeclarations &Types) {
    return JsType<T>::declare(Types) + "[]";
  }
  static bool read(const JsCall &Call, v8::Local<v8::Value> Value,
                   std::vector<T> &Into) {
    if (!JsCall::isArray(Value)) {
      return false;
    }
    Into.assign(JsCall::length(Value), T{});
    for (std::size_t Index = 0; Index < Into.size(); ++Index) {
      JsType<T>::read(Call,
                      Call.get(Value, Call.value(static_cast<double>(Index))),
                      Into[Index]);
    }
    return true;
  }
  static v8::Local<v8::Value> write(const JsCall &Call,
                                    const std::vector<T> &Value) {
    const v8::Local<v8::Object> List = Call.array();
    for (std::size_t Index = 0; Index < Value.size(); ++Index) {
      Call.set(List, Call.value(static_cast<double>(Index)),
               JsType<T>::write(Call, Value[Index]));
    }
    return List;
  }
};

template <class T> struct JsType<std::optional<T>> {
  static std::string declare(JsDeclarations &Types) {
    return JsType<T>::declare(Types) + " | undefined";
  }
  static bool read(const JsCall &Call, v8::Local<v8::Value> Value,
                   std::optional<T> &Into) {
    if (!JsType<T>::read(Call, Value, Into.emplace())) {
      Into.reset();
      return false;
    }
    return true;
  }
  static v8::Local<v8::Value> write(const JsCall &Call,
                                    const std::optional<T> &Value) {
    return Value ? JsType<T>::write(Call, *Value) : v8::Local<v8::Value>{};
  }
};

template <class T>
  requires std::is_enum_v<T>
struct JsType<T> {
  static std::string declare(JsDeclarations &Types) {
    std::string Union;
    for (const std::string_view Name : JsEnum<T>::Names) {
      Union += std::format("{}'{}'", Union.empty() ? "" : " | ", Name);
    }
    Types.try_emplace(
        std::string(JsEnum<T>::Name),
        std::format("export type {} = {};\n", JsEnum<T>::Name, Union));
    return std::string(JsEnum<T>::Name);
  }
  static bool read(const JsCall &Call, v8::Local<v8::Value> Value, T &Into) {
    if (!JsCall::isString(Value)) {
      return false;
    }
    constexpr auto &Names = JsEnum<T>::Names;
    const auto Found = std::ranges::find(Names, Call.string(Value));
    if (Found == Names.end()) {
      return false;
    }
    Into = static_cast<T>(Found - Names.begin());
    return true;
  }
  static v8::Local<v8::Value> write(const JsCall &Call, T Value) {
    return Call.value(JsEnum<T>::Names[std::to_underlying(Value)]);
  }
};

template <class T, class Each> void forEachMember(T &Value, Each &&Visit) {
  constexpr auto &Keys = glz::reflect<std::remove_const_t<T>>::keys;
  [&]<std::size_t... I>(std::index_sequence<I...>) {
    (Visit(Keys[I], glz::get<I>(glz::to_tie(Value))), ...);
  }(std::make_index_sequence<Keys.size()>{});
}

template <class T>
  requires glz::reflectable<T>
struct JsType<T> {
  static std::string declare(JsDeclarations &Types) {
    const std::string_view Full = glz::type_name<T>;
    const std::string Name(Full.substr(Full.rfind(':') + 1));
    if (!Types.try_emplace(Name).second) {
      return Name;
    }
    std::string Body = std::format("export interface {} {{\n", Name);
    T Sample{};
    forEachMember(Sample, [&]<class M>(std::string_view Key, M & /*Field*/) {
      if constexpr (IsOptional<M>) {
        Body += std::format("  {}?: {};\n", Key,
                            JsType<typename M::value_type>::declare(Types));
      } else {
        Body += std::format("  {}: {};\n", Key, JsType<M>::declare(Types));
      }
    });
    Types[Name] = Body + "}\n";
    return Name;
  }
  static bool read(const JsCall &Call, v8::Local<v8::Value> Value, T &Into) {
    if (!JsCall::isObject(Value)) {
      return false;
    }
    forEachMember(Into, [&]<class M>(std::string_view Key, M &Field) {
      JsType<M>::read(Call, Call.get(Value, Call.value(Key)), Field);
    });
    return true;
  }
  static v8::Local<v8::Value> write(const JsCall &Call, const T &Value) {
    const v8::Local<v8::Object> Object = Call.object();
    forEachMember(Value, [&]<class M>(std::string_view Key, const M &Field) {
      Call.set(Object, Call.value(Key), JsType<M>::write(Call, Field));
    });
    return Object;
  }
};

struct JsMethodInfo {
  std::string_view Feature;
  std::string_view Name;
  std::vector<std::string_view> Parameters;
  void (*Call)(const JsCall &Call);
  std::string (*Declare)(const JsMethodInfo &Method, JsDeclarations &Types);
};

void addMethod(const JsMethodInfo &Method);

template <auto Function> class JsMethod;

template <class R, class... A, R (*Function)(A...)> class JsMethod<Function> {
public:
  template <class... Names>
    requires(sizeof...(Names) == sizeof...(A) &&
             (std::convertible_to<Names, std::string_view> && ...))
  // NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
  JsMethod(std::string_view Feature, std::string_view Name,
           Names... Parameters) noexcept
      : Info{
            .Feature = Feature,
            .Name = Name,
            .Parameters = {std::string_view{Parameters}...},
            .Call = &call,
            .Declare = &declare,
        } {
    addMethod(Info);
  }
  JsMethod(const JsMethod &) = delete;
  JsMethod &operator=(const JsMethod &) = delete;

private:
  static void call(const JsCall &Call) {
    std::tuple<std::remove_cvref_t<A>...> Arguments;
    [&]<std::size_t... I>(std::index_sequence<I...>) {
      (JsType<std::remove_cvref_t<A>>::read(Call, Call.argument(I),
                                            std::get<I>(Arguments)),
       ...);
      if constexpr (std::is_void_v<R>) {
        Function(std::forward<A>(std::get<I>(Arguments))...);
      } else {
        Call.setResult(JsType<std::remove_cvref_t<R>>::write(
            Call, Function(std::forward<A>(std::get<I>(Arguments))...)));
      }
    }(std::index_sequence_for<A...>{});
  }

  static std::string declare(const JsMethodInfo &Method,
                             JsDeclarations &Types) {
    const std::array<std::string, sizeof...(A)> Kinds{
        JsType<std::remove_cvref_t<A>>::declare(Types)...};
    std::string Line = std::format("{}(", Method.Name);
    for (std::size_t Index = 0; Index < Kinds.size(); ++Index) {
      Line += std::format("{}{}: {}", Index == 0 ? "" : ", ",
                          Method.Parameters[Index], Kinds[Index]);
    }
    if constexpr (std::is_void_v<R>) {
      return Line + "): void;";
    } else {
      return Line + std::format("): {};",
                                JsType<std::remove_cvref_t<R>>::declare(Types));
    }
  }

  JsMethodInfo Info;
};

std::span<const JsMethodInfo *const> jsMethods();
void dispatchJs(const v8::FunctionCallbackInfo &Info);
std::string apiDeclarations();

} // namespace oc
