#pragma once

#include "sdk/macros.h"

namespace v8 {

class Context {
public:
  SDK_CLASS(Context);

  SDK_EXPORT_FUNCTION(V8, void(Context *), Enter, "?Enter@Context@v8@@QEAAXXZ");
  SDK_EXPORT_FUNCTION(V8, void(Context *), Exit, "?Exit@Context@v8@@QEAAXXZ");
};

} // namespace v8
