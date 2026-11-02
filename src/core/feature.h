#pragma once

#include <string_view>

namespace oc {

class Feature {
public:
  virtual ~Feature() = default;
  [[nodiscard]] virtual std::string_view name() const = 0;
  virtual bool start() { return true; }
  virtual void stop() {}
  virtual void onLevelEnd() {}
};

} // namespace oc
