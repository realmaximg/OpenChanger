#pragma once

namespace v8 {

template <class T> class Maybe {
public:
  bool has_value_;
  T value_;
};

} // namespace v8
