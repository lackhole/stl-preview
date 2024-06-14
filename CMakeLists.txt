cmake_minimum_required(VERSION 3.11)
project(preview CXX)

option(PREVIEW_TEST "Build test" OFF)

add_library(preview INTERFACE)
target_include_directories(preview INTERFACE "${CMAKE_CURRENT_LIST_DIR}/include")

if (MSVC)
    target_compile_options(preview INTERFACE /Zc:__cplusplus)
    target_compile_options(preview INTERFACE /Zc:ternary)
    target_compile_options(preview INTERFACE /permissive-)
endif ()

if (PREVIEW_TEST)
    enable_testing()
    add_subdirectory(test)
endif ()
