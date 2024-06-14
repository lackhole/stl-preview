cmake_minimum_required(VERSION 3.11)
project(preview CXX)

option(PREVIEW_TEST "Build test" OFF)

add_library(preview INTERFACE)
target_include_directories(preview INTERFACE "${CMAKE_CURRENT_LIST_DIR}/include")

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

if (PREVIEW_TEST)
    include(CTest)
    enable_testing()
    add_subdirectory(test)
endif ()
