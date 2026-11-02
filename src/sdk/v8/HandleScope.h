#pragma once

#include "sdk/macros.h"
#include "sdk/v8/Local.h"

#include <array>
#include <cstdint>

namespace v8 {

class Isolate;

class HandleScope {
public:
  SDK_CLASS(HandleScope);

  SDK_EXPORT_FUNCTION(V8, void(HandleScope *, Isolate *), Construct,
                      "??0HandleScope@v8@@QEAA@PEAVIsolate@1@@Z");
  SDK_EXPORT_FUNCTION(V8, void(HandleScope *), Destruct,
                      "??1HandleScope@v8@@QEAA@XZ");
  SDK_EXPORT_FUNCTION(
      V8, std::uintptr_t *(Isolate *, std::uintptr_t), CreateHandle,
      "?CreateHandle@HandleScope@v8@@KAPEA_KPEAVIsolate@internal@2@_K@Z");

  explicit HandleScope(Isolate *Owner) { Construct(this, Owner); }
  ~HandleScope() { Destruct(this); }
  HandleScope(const HandleScope &) = delete;
  HandleScope &operator=(const HandleScope &) = delete;

  template <class T>
  static Local<T> local(Isolate *Owner, const Global<T> &Handle) {
    return Local<T>(reinterpret_cast<T *>(
        CreateHandle(Owner, *reinterpret_cast<std::uintptr_t *>(Handle.val_))));
  }

  PAD(0x18);
};

static_assert(sizeof(HandleScope) == 0x18);

} // namespace v8
