#pragma once

#include "sdk/macros.h"
#include "sdk/v8/Local.h"
#include "sdk/v8/Value.h"

#include <cstddef>
#include <cstdint>
#include <string_view>

namespace v8 {

class Isolate;

// NOLINTNEXTLINE(performance-enum-size)
enum class NewStringType : std::int32_t { kNormal };

class String : public Value {
public:
  SDK_CLASS(String);

  SDK_EXPORT_FUNCTION(V8,
                      Local<String> *(Local<String> *, Isolate *, const char *,
                                      NewStringType, std::int32_t),
                      NewFromUtf8,
                      "?NewFromUtf8@String@v8@@SA?AV?$MaybeLocal@VString@v8@@@"
                      "2@PEAVIsolate@2@PEBDW4NewStringType@2@H@Z");

  static Local<String> fromUtf8(Isolate *Owner, std::string_view Text) {
    Local<String> Result;
    NewFromUtf8(&Result, Owner, Text.data(), NewStringType::kNormal,
                static_cast<std::int32_t>(Text.size()));
    return Result;
  }

  class Utf8Value {
  public:
    SDK_CLASS(Utf8Value);

    SDK_EXPORT_FUNCTION(V8, void(Utf8Value *, Isolate *, Local<Value>),
                        Construct,
                        "??0Utf8Value@String@v8@@QEAA@PEAVIsolate@2@V?$Local@"
                        "VValue@v8@@@2@@Z");
    SDK_EXPORT_FUNCTION(V8, void(Utf8Value *), Destruct,
                        "??1Utf8Value@String@v8@@QEAA@XZ");

    Utf8Value(Isolate *Owner, Local<Value> Item) {
      Construct(this, Owner, Item);
    }
    ~Utf8Value() { Destruct(this); }
    Utf8Value(const Utf8Value &) = delete;
    Utf8Value &operator=(const Utf8Value &) = delete;

    char *str_;
    std::int32_t length_;
  };
};

static_assert(offsetof(String::Utf8Value, str_) == 0x0);
static_assert(offsetof(String::Utf8Value, length_) == 0x8);
static_assert(sizeof(String::Utf8Value) == 0x10);

} // namespace v8
