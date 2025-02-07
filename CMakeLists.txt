cmake_minimum_required(VERSION 3.11)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

set(PREVIEW_BASE_DIR "${CMAKE_CURRENT_LIST_DIR}")
set(PREVIEW_INCLUDE_DIR "${PREVIEW_BASE_DIR}/include")
set(PREVIEW_RELATIVE_GENERATED_INCLUDE_DIR "generated/include")
set(PREVIEW_DEFAULT_GENERATED_INCLUDE_DIR "${CMAKE_CURRENT_BINARY_DIR}/${PREVIEW_RELATIVE_GENERATED_INCLUDE_DIR}")
set(PREVIEW_THIRD_PARTY_DIR "${PREVIEW_BASE_DIR}/third_party")

project(preview CXX)

option(PREVIEW_TEST "Build test" OFF)

add_library(preview INTERFACE)
target_include_directories(preview INTERFACE
        "${PREVIEW_INCLUDE_DIR}"
        "${CMAKE_CURRENT_LIST_DIR}/third_party/boost_1_86_0/include"
)
target_compile_definitions(preview INTERFACE PREVIEW_CMAKE_CONFIG)

message("CMAKE_CXX_STANDARD: ${CMAKE_CXX_STANDARD}")
message("CMAKE_TOOLCHAIN_FILE: ${CMAKE_TOOLCHAIN_FILE}")

message("CMAKE_C_COMPILER: ${CMAKE_C_COMPILER}")
message("CMAKE_C_COMPILER_VERSION: ${CMAKE_C_COMPILER_VERSION}")
message("CMAKE_CXX_COMPILER: ${CMAKE_CXX_COMPILER}")
message("CMAKE_CXX_COMPILER_VERSION: ${CMAKE_CXX_COMPILER_VERSION}")

if (MSVC)
    target_compile_options(preview INTERFACE /Zc:__cplusplus)
    target_compile_options(preview INTERFACE /Zc:ternary)
    target_compile_options(preview INTERFACE /permissive-)
endif ()

include(PreviewUtils)
include(RunConfiguration)
if (PREVIEW_TEST)
    include(CTest)
    enable_testing()

    # Test will re-configure preview via ExternalProject to execute different configuration check for every tests
    add_subdirectory(test)
else ()
    set(flags)

    if ("${CMAKE_CXX_STANDARD}" STREQUAL "")
        message(WARNING "CMAKE_CXX_STANDARD is not defined. Configuration may not behave correctly")
    else ()
        if (MSVC)
            set(flags "${flags} /std:c++${CMAKE_CXX_STANDARD}")
        else()
            set(flags "${flags} -std=c++${CMAKE_CXX_STANDARD}")
        endif ()
    endif ()

    PREVIEW_RUN_CONFIGURATION_TEST("${flags}")

    message("PREVIEW_HAVE_CXX20_ATOMIC: ${PREVIEW_USE_BOOST_ATOMIC}")
    PREVIEW_OPTION(PREVIEW_USE_BOOST_ATOMIC "Use Boost.Atomic instead of std::atomic"
            (NOT PREVIEW_HAVE_CXX20_ATOMIC)
            VISIBLE_IF (PREVIEW_HAVE_CXX20_ATOMIC)
            VERIFY     (PREVIEW_USE_BOOST_ATOMIC OR PREVIEW_HAVE_CXX20_ATOMIC)
    )
    PREVIEW_OPTION(PREVIEW_USE_SYSTEM_BOOST "Use system Boost instead of bundled Boost"
            OFF
            VISIBLE_IF (PREVIEW_USE_BOOST_ATOMIC)
            VERIFY     (Boost_FOUND)
    )
    PREVIEW_OPTION(PREVIEW_BOOST_PATH "Path to Boost"
            ""
            VISIBLE_IF (PREVIEW_USE_SYSTEM_BOOST)
            VERIFY     (Boost_FOUND)
    )
    message("PREVIEW_USE_BOOST_ATOMIC: ${PREVIEW_USE_BOOST_ATOMIC}")

    preview_verify_config()

    if (PREVIEW_USE_BOOST_ATOMIC)
        include(FindBoost.Atomic)
        target_link_libraries(preview INTERFACE Boost::atomic)
    endif ()

    PREVIEW_CONFIGURE_FILE("${PREVIEW_DEFAULT_GENERATED_INCLUDE_DIR}/preview/cmake_config.h")
    target_include_directories(preview INTERFACE "${PREVIEW_DEFAULT_GENERATED_INCLUDE_DIR}")
endif ()
