if(NOT CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR NOT CMAKE_CXX_SIMULATE_ID STREQUAL "MSVC")
    message(FATAL_ERROR "clang-cl from LLVM is required")
endif()

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_POLICY_DEFAULT_CMP0091 NEW)
set(CMAKE_MSVC_RUNTIME_LIBRARY MultiThreaded)

cmake_path(GET CMAKE_CURRENT_LIST_DIR PARENT_PATH OC_ROOT)
cmake_path(NATIVE_PATH OC_ROOT OC_ROOT_NATIVE)
add_compile_options(/clang:-march=x86-64-v2 /GS- /Zc:inline
    "/clang:-ffile-prefix-map=${OC_ROOT}=." "/clang:-ffile-prefix-map=${OC_ROOT_NATIVE}=.")
if(OPENCHANGER_DEV)
    add_compile_options(/Od)
else()
    add_compile_options(/O2 /clang:-O3 /clang:-flto /Gw /Gy)
endif()
add_link_options(/OPT:REF /OPT:ICF)

add_library(oc_options INTERFACE)
target_compile_options(oc_options INTERFACE "$<$<COMPILE_LANGUAGE:CXX>:/GR-;/EHs-c->")
target_compile_definitions(oc_options INTERFACE
    "$<$<COMPILE_LANGUAGE:CXX>:_HAS_EXCEPTIONS=0>"
    "$<$<BOOL:${OPENCHANGER_DEV}>:OPENCHANGER_DEV>"
)
