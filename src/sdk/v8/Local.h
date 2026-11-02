#pragma once

#include <type_traits>

namespace v8 {

template <class T> class Local {
public:
  Local() = default;
  explicit Local(T *Slot) : val_(Slot) {}
  template <class S>
    requires std::is_base_of_v<T, S>
  // NOLINTNEXTLINE(misc-explicit-constructor)
  Local(Local<S> Other) : val_(Other.val_) {}

  T *val_ = nullptr;
};

template <class T> class Global {
public:
  T *val_;
};

} // namespace v8
