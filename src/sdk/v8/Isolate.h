#pragma once

#include "sdk/macros.h"
#include "sdk/v8/Local.h"

namespace v8 {

class Context;

class Isolate {
public:
  SDK_CLASS(Isolate);

  SDK_EXPORT_FUNCTION(V8, void(Isolate *), Enter, "?Enter@Isolate@v8@@QEAAXXZ");
  SDK_EXPORT_FUNCTION(V8, void(Isolate *), Exit, "?Exit@Isolate@v8@@QEAAXXZ");
  SDK_EXPORT_FUNCTION(
      V8, Local<Context> *(Isolate *, Local<Context> *), GetCurrentContext,
      "?GetCurrentContext@Isolate@v8@@QEAA?AV?$Local@VContext@v8@@@2@XZ");
};

} // namespace v8
