add_library(oc_changer STATIC
    catalog/catalog.cpp
    inventory/objects.cpp
    loadout/loadout.cpp
    loadout/validate.cpp
    voice/voices.cpp
    ${OC_PROTO_SOURCES}
)
add_dependencies(oc_changer oc_proto_generate)
target_include_directories(oc_changer PUBLIC "${OC_PROTO_GEN_DIR}")
target_link_libraries(oc_changer PUBLIC oc_core protobuf::libprotobuf)
target_link_libraries(oc_objects PUBLIC oc_changer)
target_sources(oc_objects PRIVATE
    Changer.cpp
    catalog/build.cpp
    inspect/ReflectWeaponState.cpp
    inventory/AdjustEquipSlots.cpp
    inventory/ApplySOCacheSubscribed.cpp
    inventory/SOCreated.cpp
    inventory/SOUpdated.cpp
    inventory/socache.cpp
    skins/decoded.cpp
    stattrak/player_death.cpp
    stattrak/round_mvp.cpp
    voice/DoStartSoundEvent.cpp
    voice/grenade_thrown.cpp
)
