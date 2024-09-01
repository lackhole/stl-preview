cmake_minimum_required(VERSION 3.11)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

set(PREVIEW_BASE_DIR "${CMAKE_CURRENT_LIST_DIR}")
set(PREVIEW_INCLUDE_DIR "${PREVIEW_BASE_DIR}/include")
set(PREVIEW_RELATIVE_GENERATED_INCLUDE_DIR "generated/include")
set(PREVIEW_DEFAULT_GENERATED_INCLUDE_DIR "${CMAKE_CURRENT_BINARY_DIR}/${PREVIEW_RELATIVE_GENERATED_INCLUDE_DIR}")

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

include(RunConfiguration)
if (PREVIEW_TEST)
    include(CTest)
    enable_testing()

    # Test will re-configure preview via ExternalProject to execute different RunConfiguration() for every tests
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

    RunConfiguration("${PREVIEW_DEFAULT_GENERATED_INCLUDE_DIR}/preview/cmake_config.h" "${flags}")
    target_include_directories(preview INTERFACE "${PREVIEW_DEFAULT_GENERATED_INCLUDE_DIR}")
endif ()
