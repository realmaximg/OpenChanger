#pragma once

#include "sdk/macros.h"

#include <google/protobuf/message_lite.h>

#define PtApplySOCacheSubscribed                                               \
  "48 89 5C 24 18 48 89 74 24 20 57 48 83 EC 60 33 FF 48 8B DA F6 42 10 01 "   \
  "48 8B F1 0F 84"

namespace GCSDK {

class CGCClient {
public:
  SDK_CLASS(CGCClient);

  PATTERN_FUNCTION(Client, void(CGCClient *, google::protobuf::MessageLite *),
                   ApplySOCacheSubscribed, PtApplySOCacheSubscribed);
};

} // namespace GCSDK
