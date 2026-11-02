#include "features/changer/inventory/socache.h"
#include "game/hook.h"
#include "gcsdk_gcmessages.pb.h"
#include "sdk/client/CCSInventoryManager.h"
#include "sdk/client/CGCClient.h"
#include "sdk/client/SOID_t.h"

#include <google/protobuf/message_lite.h>

namespace oc::changer {

namespace {

void detour(GCSDK::CGCClient *Client, google::protobuf::MessageLite *Message);

InlineHook<detour> Hook{GCSDK::CGCClient::ApplySOCacheSubscribedLink};

void detour(GCSDK::CGCClient *Client, google::protobuf::MessageLite *Message) {
  const auto &Subscribed = *static_cast<const CMsgSOCacheSubscribed *>(Message);
  const bool HasOwner = Subscribed.has_owner_soid();
  const GCSDK::SOID_t Owner{
      .m_id = Subscribed.owner_soid().id(),
      .m_type = Subscribed.owner_soid().type(),
  };
  Hook.original(Client, Message);
  if (HasOwner && CCSInventoryManager::get()->isLocalOwner(Owner)) {
    resubscribed();
  }
}

} // namespace

} // namespace oc::changer
