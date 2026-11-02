#include "features/changer/inventory/socache.h"
#include "game/hook.h"
#include "sdk/client/CPlayerInventory.h"
#include "sdk/client/CSharedObject.h"
#include "sdk/client/CSharedObjectCache.h"
#include "sdk/client/SOID_t.h"

namespace oc::changer {

namespace {

void detour(CPlayerInventory *Inventory, const GCSDK::SOID_t *Owner,
            GCSDK::CSharedObject *Object, GCSDK::ESOCacheEvent Event);

InlineHook<detour> Hook{CPlayerInventory::SOUpdatedLink};

void detour(CPlayerInventory *Inventory, const GCSDK::SOID_t *Owner,
            GCSDK::CSharedObject *Object, GCSDK::ESOCacheEvent Event) {
  Hook.original(Inventory, Owner, Object, Event);
  realSlotChanged(*Owner, *Object, Event);
}

} // namespace

} // namespace oc::changer
