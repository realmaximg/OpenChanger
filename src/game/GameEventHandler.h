#pragma once

#include "sdk/client/IGameEvent.h"

#include <string_view>

namespace oc {

class GameEventHandler {
public:
  using Callback = void (*)(IGameEvent &);

  GameEventHandler(std::string_view Name, Callback Handler) noexcept;
  GameEventHandler(const GameEventHandler &) = delete;
  GameEventHandler &operator=(const GameEventHandler &) = delete;

private:
  friend void dispatchGameEvent(IGameEvent &Event);

  std::string_view Name;
  Callback Handler;
};

void dispatchGameEvent(IGameEvent &Event);

} // namespace oc
