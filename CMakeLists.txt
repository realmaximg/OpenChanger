cmake_minimum_required(VERSION 3.25)
project(openchanger LANGUAGES C CXX)

option(OPENCHANGER_DEV "Development code (log, dev exports)" OFF)
set(OC_PROTOBUF_PREFIX "" CACHE PATH "")

include(cmake/flags.cmake)

find_package(protobuf CONFIG REQUIRED PATHS "${OC_PROTOBUF_PREFIX}" NO_DEFAULT_PATH)

add_library(glaze INTERFACE)
target_include_directories(glaze SYSTEM INTERFACE thirdparty/glaze/include)

set(LIBHAT_TESTING OFF CACHE BOOL "" FORCE)
set(LIBHAT_EXAMPLES OFF CACHE BOOL "" FORCE)
set(LIBHAT_INSTALL_TARGET OFF CACHE BOOL "" FORCE)
set(LIBHAT_WARNINGS_AS_ERRORS OFF CACHE BOOL "" FORCE)
add_subdirectory(thirdparty/libhat EXCLUDE_FROM_ALL SYSTEM)
target_link_libraries(libhat PRIVATE oc_options)

add_library(safetyhook STATIC thirdparty/safetyhook/safetyhook.cpp thirdparty/safetyhook/Zydis.c)
target_include_directories(safetyhook SYSTEM PUBLIC thirdparty/safetyhook)
target_link_libraries(safetyhook PRIVATE oc_options)

set(OC_PROTO_DIR "${CMAKE_CURRENT_SOURCE_DIR}/proto")
set(OC_PROTO_GEN_DIR "${CMAKE_CURRENT_BINARY_DIR}/generated")
set(OC_PROTO_NAMES steammessages base_gcmessages gcsdk_gcmessages openchanger)

foreach(name IN LISTS OC_PROTO_NAMES)
    list(APPEND OC_PROTO_FILES "${OC_PROTO_DIR}/${name}.proto")
    list(APPEND OC_PROTO_SOURCES "${OC_PROTO_GEN_DIR}/${name}.pb.cc")
    list(APPEND OC_PROTO_OUTPUTS "${OC_PROTO_GEN_DIR}/${name}.pb.cc" "${OC_PROTO_GEN_DIR}/${name}.pb.h")
endforeach()
file(MAKE_DIRECTORY "${OC_PROTO_GEN_DIR}")
add_custom_command(
    OUTPUT ${OC_PROTO_OUTPUTS}
    COMMAND protobuf::protoc
        "--proto_path=${OC_PROTO_DIR}"
        "--proto_path=${OC_PROTOBUF_PREFIX}/include"
        "--cpp_out=${OC_PROTO_GEN_DIR}"
        ${OC_PROTO_FILES}
    DEPENDS protobuf::protoc ${OC_PROTO_FILES}
    COMMENT "protoc: proto/*.proto -> generated/"
    VERBATIM
)
add_custom_target(oc_proto_generate DEPENDS ${OC_PROTO_OUTPUTS})

add_library(oc_core STATIC
    src/core/log.cpp
    src/core/settings.cpp
)
target_include_directories(oc_core PUBLIC src)
target_link_libraries(oc_core PUBLIC glaze oc_options)

add_library(oc_objects OBJECT
    src/app/features.cpp
    src/app/LoopDeactivateAllSystems.cpp
    src/game/Decode.cpp
    src/game/DecodedHandler.cpp
    src/game/find.cpp
    src/game/FireEventClientSide.cpp
    src/game/GameEventHandler.cpp
    src/game/hook.cpp
    src/game/js.cpp
    src/game/link.cpp
    src/game/module.cpp
    src/game/panorama.cpp
    src/game/pattern.cpp
    src/sdk/client/CCSPlayerController.cpp
    src/sdk/client/CGameEntitySystem.cpp
    src/sdk/client/CModel.cpp
    src/sdk/client/CModelState.cpp
    src/sdk/client/C_BasePlayerPawn.cpp
    src/sdk/client/C_CSPlayerPawn.cpp
    src/sdk/networksystem/CNetworkFieldChangedEventQueue.cpp
    src/sdk/scenefilecache/CResponseSystem.cpp
    src/sdk/tier0/CAtomicMutex.cpp
    src/ui/changer/methods.cpp
    src/ui/ui.cpp
)
target_link_libraries(oc_objects PUBLIC oc_core libhat safetyhook)

add_library(openchanger SHARED src/app/dllmain.cpp)
target_link_libraries(openchanger PRIVATE oc_objects)

set(OC_UI_DIST "${CMAKE_CURRENT_SOURCE_DIR}/src/ui/panorama/dist")
if(OPENCHANGER_DEV)
    target_sources(openchanger PRIVATE src/app/dev.cpp)
    target_compile_definitions(oc_objects PRIVATE "OC_UI_DIST=\"${OC_UI_DIST}\"")
    set(OC_API_TYPES "${CMAKE_CURRENT_SOURCE_DIR}/src/ui/panorama/generated/api.d.ts")
    add_executable(oc_api_types src/app/api_types.cpp)
    target_link_libraries(oc_api_types PRIVATE oc_objects)
    add_custom_command(
        OUTPUT "${OC_API_TYPES}"
        COMMAND oc_api_types "${OC_API_TYPES}"
        DEPENDS oc_api_types
        COMMENT "oc_api_types: the JS method registry -> src/ui/panorama/generated/api.d.ts"
        VERBATIM
    )
    add_custom_target(oc_api_types_file ALL DEPENDS "${OC_API_TYPES}")
else()
    foreach(file bundle.js bundle.css)
        if(NOT EXISTS "${OC_UI_DIST}/${file}")
            message(FATAL_ERROR "${OC_UI_DIST}/${file} is missing: build the frontend first (npm run build:release in src/ui/panorama)")
        endif()
    endforeach()
    set_source_files_properties(src/ui/ui.cpp PROPERTIES
        COMPILE_DEFINITIONS "OC_UI_BUNDLE_JS=\"${OC_UI_DIST}/bundle.js\";OC_UI_BUNDLE_CSS=\"${OC_UI_DIST}/bundle.css\""
        COMPILE_OPTIONS "-Wno-c23-extensions"
        OBJECT_DEPENDS "${OC_UI_DIST}/bundle.js;${OC_UI_DIST}/bundle.css"
    )
endif()

add_subdirectory(src/features/changer)
