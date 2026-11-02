#pragma once

#include "sdk/macros.h"
#include "sdk/v8/Local.h"
#include "sdk/v8/Value.h"

namespace v8 {

class Isolate;

class Boolean : public Value {
public:
  SDK_CLASS(Boolean);

  SDK_EXPORT_FUNCTION(
      V8, Local<Boolean> *(Local<Boolean> *, Isolate *, bool), New,
      "?New@Boolean@v8@@SA?AV?$Local@VBoolean@v8@@@2@PEAVIsolate@2@_N@Z");
};

} // namespace v8
