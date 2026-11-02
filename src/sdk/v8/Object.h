#pragma once

#include "sdk/macros.h"
#include "sdk/v8/Local.h"
#include "sdk/v8/Maybe.h"
#include "sdk/v8/Value.h"

namespace v8 {

class Context;
class Isolate;

class Object : public Value {
public:
  SDK_CLASS(Object);

  SDK_EXPORT_FUNCTION(
      V8, Local<Object> *(Local<Object> *, Isolate *), New,
      "?New@Object@v8@@SA?AV?$Local@VObject@v8@@@2@PEAVIsolate@2@@Z");
  SDK_EXPORT_FUNCTION(V8,
                      Maybe<bool> *(Object *, Maybe<bool> *, Local<Context>,
                                    Local<Value>, Local<Value>),
                      Set,
                      "?Set@Object@v8@@QEAA?AV?$Maybe@_N@2@V?$Local@VContext@"
                      "v8@@@2@V?$Local@VValue@v8@@@2@1@Z");
  SDK_EXPORT_FUNCTION(V8,
                      Local<Value> *(Object *, Local<Value> *, Local<Context>,
                                     Local<Value>),
                      Get,
                      "?Get@Object@v8@@QEAA?AV?$MaybeLocal@VValue@v8@@@2@V?$"
                      "Local@VContext@v8@@@2@V?$Local@VValue@v8@@@2@@Z");
};

} // namespace v8
