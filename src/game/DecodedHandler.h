#pragma once

#include "sdk/client/CEntityInstance.h"
#include "sdk/networksystem/CNetworkFieldChangedEventQueue.h"

namespace oc {

class DecodedHandler {
public:
  using Callback = void (*)(CEntityInstance &,
                            CNetworkFieldChangedEventQueue *);

  explicit DecodedHandler(Callback Handler) noexcept;
  DecodedHandler(const DecodedHandler &) = delete;
  DecodedHandler &operator=(const DecodedHandler &) = delete;

private:
  friend void dispatchDecoded(CEntityInstance &Entity,
                              CNetworkFieldChangedEventQueue *Changes);

  Callback Handler;
};

void dispatchDecoded(CEntityInstance &Entity,
                     CNetworkFieldChangedEventQueue *Changes);

} // namespace oc
