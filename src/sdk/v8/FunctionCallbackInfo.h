#pragma once

#include "sdk/v8/Local.h"
#include "sdk/v8/Value.h"

#include <cstddef>
#include <cstdint>

namespace v8 {

class Isolate;

class FunctionCallbackInfo {
public:
  static constexpr std::int32_t kIsolateIndex = 1;
  static constexpr std::int32_t kReturnValueIndex = 3;

  [[nodiscard]] Isolate *isolate() const {
    return static_cast<Isolate *>(implicit_args_[kIsolateIndex]);
  }

  [[nodiscard]] Local<Value> argument(std::int32_t Index) const {
    if (Index >= length_) {
      return {};
    }
    return Local<Value>(reinterpret_cast<Value *>(values_ + Index));
  }

  void setReturnValue(Local<Value> Result) const {
    implicit_args_[kReturnValueIndex] = *reinterpret_cast<void **>(Result.val_);
  }

  void **implicit_args_;
  void **values_;
  std::int32_t length_;
};

static_assert(offsetof(FunctionCallbackInfo, implicit_args_) == 0x0);
static_assert(offsetof(FunctionCallbackInfo, values_) == 0x8);
static_assert(offsetof(FunctionCallbackInfo, length_) == 0x10);

} // namespace v8
