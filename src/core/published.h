#pragma once

#include <atomic>
#include <memory>
#include <mutex>
#include <utility>

namespace oc {

template <class T> class Published {
public:
  [[nodiscard]] std::shared_ptr<const T> get() const { return Current.load(); }

  template <class F> void update(F Change) {
    const std::scoped_lock Lock(Writer);
    auto Copy = std::make_shared<T>(*Current.load());
    Change(*Copy);
    Current.store(std::move(Copy));
  }

private:
  std::atomic<std::shared_ptr<const T>> Current{std::make_shared<const T>()};
  std::mutex Writer;
};

} // namespace oc
