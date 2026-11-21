#pragma once

#include "sdk/client/CEntityInstance.h"
#include "sdk/networksystem/CNetworkFieldChangedEventQueue.h"

#include <cstdint>

namespace oc {

class DecodedHandler {
public:
  using Callback = void (*)(CEntityInstance &,
                            CNetworkFieldChangedEventQueue *);

  explicit DecodedHandler(Callback Handler) noexcept;
  DecodedHandler(const DecodedHandler &) = delete;
  DecodedHandler &operator=(const DecodedHandler &) = delete;

private:
  friend void dispatchDecoded(std::int32_t Index,
                              CNetworkFieldChangedEventQueue *Changes);

  Callback Handler;
};

void dispatchDecoded(std::int32_t Index,
                     CNetworkFieldChangedEventQueue *Changes);

} // namespace oc
