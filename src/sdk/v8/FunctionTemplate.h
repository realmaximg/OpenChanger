#pragma once

#include "sdk/macros.h"
#include "sdk/v8/FunctionCallbackInfo.h"
#include "sdk/v8/Local.h"

#include <cstdint>

namespace v8 {

class CFunction;
class Isolate;
class Signature;
class Value;

// NOLINTNEXTLINE(performance-enum-size)
enum class ConstructorBehavior : std::int32_t { kThrow, kAllow };
// NOLINTNEXTLINE(performance-enum-size)
enum class SideEffectType : std::int32_t { kHasSideEffect };

using FunctionCallback = void (*)(const FunctionCallbackInfo &Info);

class FunctionTemplate {
public:
  SDK_CLASS(FunctionTemplate);

  SDK_EXPORT_FUNCTION(
      V8,
      Local<FunctionTemplate> *(
          Local<FunctionTemplate> *, Isolate *, FunctionCallback, Local<Value>,
          Local<Signature>, std::int32_t, ConstructorBehavior, SideEffectType,
          const CFunction *, std::uint16_t, std::uint16_t, std::uint16_t),
      New,
      "?New@FunctionTemplate@v8@@SA?AV?$Local@VFunctionTemplate@v8@@@2@"
      "PEAVIsolate@2@P6AXAEBV?$FunctionCallbackInfo@VValue@v8@@@2@@ZV?$"
      "Local@VValue@v8@@@2@V?$Local@VSignature@v8@@@2@HW4ConstructorBehavior@"
      "2@W4SideEffectType@2@PEBVCFunction@2@GGG@Z");
};

} // namespace v8
