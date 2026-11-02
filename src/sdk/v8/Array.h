#pragma once

#include "sdk/macros.h"
#include "sdk/v8/Local.h"
#include "sdk/v8/Object.h"

#include <cstdint>

namespace v8 {

class Isolate;

class Array : public Object {
public:
  SDK_CLASS(Array);

  SDK_EXPORT_FUNCTION(
      V8, Local<Array> *(Local<Array> *, Isolate *, std::int32_t), New,
      "?New@Array@v8@@SA?AV?$Local@VArray@v8@@@2@PEAVIsolate@2@H@Z");
  SDK_EXPORT_FUNCTION(V8, std::uint32_t(Array *), Length,
                      "?Length@Array@v8@@QEBAIXZ");
};

} // namespace v8
