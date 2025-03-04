cmake_minimum_required(VERSION 3.11)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

if (POLICY CMP0135)
    cmake_policy(SET CMP0135 NEW)
endif ()

set(PREVIEW_BASE_DIR "${CMAKE_CURRENT_LIST_DIR}")
set(PREVIEW_INCLUDE_DIR "${PREVIEW_BASE_DIR}/include")
set(PREVIEW_RELATIVE_GENERATED_INCLUDE_DIR "generated/include")
set(PREVIEW_DEFAULT_GENERATED_INCLUDE_DIR "${CMAKE_CURRENT_BINARY_DIR}/${PREVIEW_RELATIVE_GENERATED_INCLUDE_DIR}")

project(preview CXX)

option(PREVIEW_TEST "Build test" OFF)

add_library(preview INTERFACE)
target_include_directories(preview INTERFACE "${PREVIEW_INCLUDE_DIR}")

# Notify that the project is built with CMake configuration
target_compile_definitions(preview INTERFACE PREVIEW_CMAKE_CONFIG)

if (MSVC)
    target_compile_options(preview INTERFACE /Zc:__cplusplus)
    target_compile_options(preview INTERFACE /Zc:ternary)
    target_compile_options(preview INTERFACE /permissive-)
endif ()

include(PreviewConfiguration)

PREVIEW_RUN_COMPILE_CHECKS()

set(PREVIEW_CXX20_SPECIFIED FALSE)
if (NOT "${CMAKE_CXX_STANDARD}" STREQUAL "")
    if (CMAKE_CXX_STANDARD GREATER_EQUAL 20)
        set(PREVIEW_CXX20_SPECIFIED TRUE)
    endif ()
endif ()

PREVIEW_OPTION(PREVIEW_PREFER_STL_ATOMIC "Prefer STL atomic if possible" ON)
PREVIEW_OPTION(PREVIEW_USE_BOOST_ATOMIC "Use Boost.Atomic"
        ((NOT ${PREVIEW_PREFER_STL_ATOMIC}) OR (NOT PREVIEW_CXX20_SPECIFIED))
)
set(PREVIEW_BOOST_ROOT "" CACHE PATH "Path to Boost root directory. If not set, Boost will be downloaded and built.")

if (PREVIEW_USE_BOOST_ATOMIC)
    include(FindBoost.Atomic)
    target_link_libraries(preview INTERFACE Boost::atomic)
endif ()

preview_verify_config()

PREVIEW_CONFIGURE_FILE("${PREVIEW_DEFAULT_GENERATED_INCLUDE_DIR}/preview/cmake_config.h")
target_include_directories(preview INTERFACE "${PREVIEW_DEFAULT_GENERATED_INCLUDE_DIR}")

if (PREVIEW_TEST)
    include(CTest)
    enable_testing()
    add_subdirectory(test)
endif ()

message("CMAKE_C_COMPILER: ${CMAKE_C_COMPILER}")
message("CMAKE_C_COMPILER_VERSION: ${CMAKE_C_COMPILER_VERSION}")
message("CMAKE_C_FLAGS: ${CMAKE_C_FLAGS}")
message("CMAKE_CXX_COMPILER: ${CMAKE_CXX_COMPILER}")
message("CMAKE_CXX_COMPILER_VERSION: ${CMAKE_CXX_COMPILER_VERSION}")
message("CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")
message("CMAKE_TOOLCHAIN_FILE: ${CMAKE_TOOLCHAIN_FILE}")
