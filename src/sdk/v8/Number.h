#pragma once

#include "sdk/macros.h"
#include "sdk/v8/Local.h"
#include "sdk/v8/Value.h"

namespace v8 {

class Isolate;

class Number : public Value {
public:
  SDK_CLASS(Number);

  SDK_EXPORT_FUNCTION(
      V8, Local<Number> *(Local<Number> *, Isolate *, double), New,
      "?New@Number@v8@@SA?AV?$Local@VNumber@v8@@@2@PEAVIsolate@2@N@Z");
};

} // namespace v8
