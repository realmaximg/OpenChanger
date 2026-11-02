#pragma once

#include "sdk/macros.h"
#include "sdk/v8/Local.h"
#include "sdk/v8/Maybe.h"

#include <cstdint>

namespace v8 {

class Context;
class Isolate;

class Value {
public:
  SDK_CLASS(Value);

  SDK_EXPORT_FUNCTION(
      V8, Maybe<std::int64_t> *(Value *, Maybe<std::int64_t> *, Local<Context>),
      IntegerValue,
      "?IntegerValue@Value@v8@@QEBA?AV?$Maybe@_J@2@V?$Local@VContext@v8@@@2@@"
      "Z");
  SDK_EXPORT_FUNCTION(
      V8, Maybe<double> *(Value *, Maybe<double> *, Local<Context>),
      NumberValue,
      "?NumberValue@Value@v8@@QEBA?AV?$Maybe@N@2@V?$Local@VContext@v8@@@2@@Z");
  SDK_EXPORT_FUNCTION(V8, bool(Value *, Isolate *), BooleanValue,
                      "?BooleanValue@Value@v8@@QEBA_NPEAVIsolate@2@@Z");
  SDK_EXPORT_FUNCTION(V8, bool(Value *), IsFunction,
                      "?IsFunction@Value@v8@@QEBA_NXZ");
  SDK_EXPORT_FUNCTION(V8, bool(Value *), IsNumber,
                      "?IsNumber@Value@v8@@QEBA_NXZ");
  SDK_EXPORT_FUNCTION(V8, bool(Value *), IsString,
                      "?IsString@Value@v8@@QEBA_NXZ");
  SDK_EXPORT_FUNCTION(V8, bool(Value *), IsBoolean,
                      "?IsBoolean@Value@v8@@QEBA_NXZ");
  SDK_EXPORT_FUNCTION(V8, bool(Value *), IsArray,
                      "?IsArray@Value@v8@@QEBA_NXZ");
  SDK_EXPORT_FUNCTION(V8, bool(Value *), IsObject,
                      "?IsObject@Value@v8@@QEBA_NXZ");
};

} // namespace v8
