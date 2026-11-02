#include "game/js.h"

#include "core/log.h"
#include "sdk/v8/Array.h"
#include "sdk/v8/Boolean.h"
#include "sdk/v8/Context.h"
#include "sdk/v8/FunctionCallbackInfo.h"
#include "sdk/v8/HandleScope.h"
#include "sdk/v8/Isolate.h"
#include "sdk/v8/Local.h"
#include "sdk/v8/Maybe.h"
#include "sdk/v8/Number.h"
#include "sdk/v8/Object.h"
#include "sdk/v8/String.h"
#include "sdk/v8/Value.h"

#include <cstddef>
#include <cstdint>
#include <format>
#include <map>
#include <ranges>
#include <span>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace oc {

namespace {

std::vector<const JsMethodInfo *> &registry() {
  static std::vector<const JsMethodInfo *> Methods;
  return Methods;
}

} // namespace

void addMethod(const JsMethodInfo &Method) { registry().push_back(&Method); }

std::span<const JsMethodInfo *const> jsMethods() { return registry(); }

void dispatchJs(const v8::FunctionCallbackInfo &Info) {
  v8::Isolate *const Isolate = Info.isolate();
  if (Info.length_ < 1) {
    return;
  }
  const v8::HandleScope Scope(Isolate);
  v8::Local<v8::Context> Context;
  v8::Isolate::GetCurrentContext(Isolate, &Context);
  v8::Maybe<std::int64_t> Number{};
  v8::Value::IntegerValue(Info.argument(0).val_, &Number, Context);
  const auto &Methods = registry();
  if (!Number.has_value_ || Number.value_ < 0 ||
      std::cmp_greater_equal(Number.value_, Methods.size())) {
    OC_LOG("panorama", "no method {}", Number.value_);
    return;
  }
  const JsCall Call(Info, Context);
  Methods[static_cast<std::size_t>(Number.value_)]->Call(Call);
}

std::string apiDeclarations() {
  JsDeclarations Types;
  std::map<std::string_view, std::string> Features;
  for (const JsMethodInfo *const Method : registry()) {
    Features[Method->Feature] +=
        std::format("    {}\n", Method->Declare(*Method, Types));
  }
  std::string Text;
  for (const std::string &Type : Types | std::views::values) {
    Text += Type + "\n";
  }
  Text += "export interface OpenChangerMethods {\n";
  for (const auto &[Feature, Methods] : Features) {
    Text += std::format("  {}: {{\n{}  }};\n", Feature, Methods);
  }
  return Text + "}\n";
}

JsCall::JsCall(const v8::FunctionCallbackInfo &Info,
               v8::Local<v8::Context> Context) noexcept
    : Info(&Info), Context(Context) {}

v8::Local<v8::Value> JsCall::argument(std::size_t Index) const {
  return Info->argument(static_cast<std::int32_t>(Index + 1));
}

void JsCall::setResult(v8::Local<v8::Value> Value) const {
  if (Value.val_ != nullptr) {
    Info->setReturnValue(Value);
  }
}

bool JsCall::isNumber(v8::Local<v8::Value> Value) {
  return Value.val_ != nullptr && v8::Value::IsNumber(Value.val_);
}

bool JsCall::isString(v8::Local<v8::Value> Value) {
  return Value.val_ != nullptr && v8::Value::IsString(Value.val_);
}

bool JsCall::isBoolean(v8::Local<v8::Value> Value) {
  return Value.val_ != nullptr && v8::Value::IsBoolean(Value.val_);
}

bool JsCall::isArray(v8::Local<v8::Value> Value) {
  return Value.val_ != nullptr && v8::Value::IsArray(Value.val_);
}

bool JsCall::isObject(v8::Local<v8::Value> Value) {
  return Value.val_ != nullptr && v8::Value::IsObject(Value.val_);
}

bool JsCall::isFunction(v8::Local<v8::Value> Value) {
  return Value.val_ != nullptr && v8::Value::IsFunction(Value.val_);
}

std::int64_t JsCall::integer(v8::Local<v8::Value> Value) const {
  v8::Maybe<std::int64_t> Result{};
  v8::Value::IntegerValue(Value.val_, &Result, Context);
  return Result.has_value_ ? Result.value_ : 0;
}

double JsCall::number(v8::Local<v8::Value> Value) const {
  v8::Maybe<double> Result{};
  v8::Value::NumberValue(Value.val_, &Result, Context);
  return Result.has_value_ ? Result.value_ : 0.0;
}

std::string JsCall::string(v8::Local<v8::Value> Value) const {
  const v8::String::Utf8Value Text(Info->isolate(), Value);
  if (Text.str_ == nullptr) {
    return {};
  }
  return {Text.str_, static_cast<std::size_t>(Text.length_)};
}

bool JsCall::boolean(v8::Local<v8::Value> Value) const {
  return v8::Value::BooleanValue(Value.val_, Info->isolate());
}

std::uint32_t JsCall::length(v8::Local<v8::Value> Array) {
  return v8::Array::Length(reinterpret_cast<v8::Array *>(Array.val_));
}

v8::Local<v8::Value> JsCall::get(v8::Local<v8::Value> Object,
                                 v8::Local<v8::Value> Key) const {
  v8::Local<v8::Value> Result;
  v8::Object::Get(reinterpret_cast<v8::Object *>(Object.val_), &Result, Context,
                  Key);
  return Result;
}

v8::Local<v8::Value> JsCall::value(double Value) const {
  v8::Local<v8::Number> Result;
  v8::Number::New(&Result, Info->isolate(), Value);
  return Result;
}

v8::Local<v8::Value> JsCall::value(std::string_view Value) const {
  return v8::String::fromUtf8(Info->isolate(), Value);
}

v8::Local<v8::Value> JsCall::truth(bool Value) const {
  v8::Local<v8::Boolean> Result;
  v8::Boolean::New(&Result, Info->isolate(), Value);
  return Result;
}

v8::Local<v8::Object> JsCall::array() const {
  v8::Local<v8::Array> Handle;
  v8::Array::New(&Handle, Info->isolate(), std::int32_t{0});
  return Handle;
}

v8::Local<v8::Object> JsCall::object() const {
  v8::Local<v8::Object> Handle;
  v8::Object::New(&Handle, Info->isolate());
  return Handle;
}

void JsCall::set(v8::Local<v8::Object> Target, v8::Local<v8::Value> Key,
                 v8::Local<v8::Value> Item) const {
  if (Item.val_ == nullptr) {
    return;
  }
  v8::Maybe<bool> Done{};
  v8::Object::Set(Target.val_, &Done, Context, Key, Item);
}

} // namespace oc
