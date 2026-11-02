#pragma once

#include "features/changer/inventory/objects.h"
#include "sdk/client/CSharedObject.h"
#include "sdk/client/CSharedObjectCache.h"
#include "sdk/client/SOID_t.h"

#include <vector>

namespace oc::changer {

void requestSync();
void resubscribed();
void realSlotChanged(const GCSDK::SOID_t &Owner, GCSDK::CSharedObject &Object,
                     GCSDK::ESOCacheEvent Event);
std::vector<EquipSlot> realSlots();

} // namespace oc::changer
