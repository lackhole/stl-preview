# Check maximum C++ version that compiler supports
include(CheckCXXSourceCompiles)
include(CheckCXXCompilerFlag)
include(ExternalProject)

message("Configuring tests...")

if (MSVC)
    set(_PREVIEW_CXX_STD_FLAG /std:c++)
else()
    set(_PREVIEW_CXX_STD_FLAG -std=c++)
endif()
set(__cplusplus11 "201103L")
set(__cplusplus14 "201402L")
set(__cplusplus17 "201703L")
set(__cplusplus20 "202002L")
set(__cplusplus23 "202302L")

macro(preview_check_compiler_support_std version out)
    check_cxx_compiler_flag("${_PREVIEW_CXX_STD_FLAG}${version}" PREVIEW_COMPILER_FLAG_CXX_STD${version})

    if (NOT ${PREVIEW_COMPILER_FLAG_CXX_STD${version}})
        set(${out} 0)
    else()

        set(CMAKE_REQUIRED_FLAGS "${_PREVIEW_CXX_STD_FLAG}${version}")
        if (MSVC)
            set(CMAKE_REQUIRED_FLAGS "${CMAKE_REQUIRED_FLAGS} /Zc:__cplusplus")
        endif()
        set(_source "
#if __cplusplus < ${__cplusplus${version}}
#error 1
#endif
int main() { return 0; }")
        check_cxx_source_compiles("${_source}" ${out})
        set(CMAKE_REQUIRED_FLAGS)
    endif ()
endmacro()

macro(target_compile_option_if_possible target flag)
    check_cxx_compiler_flag("${flag}" CHECK_CXX_FLAG_${flag})
    if (CHECK_CXX_FLAG_${flag})
        target_compile_options(${target} PRIVATE "${flag}")
    endif()
endmacro()

preview_check_compiler_support_std(14 PREVIEW_COMPILER_SUPPORT_CXX14)
if (NOT PREVIEW_COMPILER_SUPPORT_CXX14)
    message(FATAL_ERROR "Compiler must support C++14")
endif ()
set(std_list 14)

preview_check_compiler_support_std(17 PREVIEW_COMPILER_SUPPORT_CXX17)
if (PREVIEW_COMPILER_SUPPORT_CXX17)
    list(APPEND std_list 17)
    preview_check_compiler_support_std(20 PREVIEW_COMPILER_SUPPORT_CXX20)
endif ()
if (PREVIEW_COMPILER_SUPPORT_CXX20)
    list(APPEND std_list 20)
    preview_check_compiler_support_std(23 PREVIEW_COMPILER_SUPPORT_CXX23)
endif ()
if (PREVIEW_COMPILER_SUPPORT_CXX23)
    list(APPEND std_list 23)
#    preview_check_compiler_support_std(26 PREVIEW_COMPILER_SUPPORT_CXX26)
endif ()
message("Supported C++ standard: ${std_list}")


# Get googletest
if (ANDROID)
    message("ANDROID_NDK: ${ANDROID_NDK}")
    set(GOOGLETEST_ROOT ${ANDROID_NDK}/sources/third_party/googletest)
    message("Looking for googletest in ${GOOGLETEST_ROOT}")
    add_library(gtest STATIC ${GOOGLETEST_ROOT}/src/gtest_main.cc ${GOOGLETEST_ROOT}/src/gtest-all.cc)
    target_include_directories(gtest PRIVATE ${GOOGLETEST_ROOT})
    target_include_directories(gtest PUBLIC ${GOOGLETEST_ROOT}/include)
    add_library(GTest::gtest_main ALIAS gtest)

#    if ("${ANDROID_SDK}" STREQUAL "")
#        set(ANDROID_SDK "${ANDROID_NDK}/../..")
#    endif ()
#    find_program(ADB adb REQUIRED PATHS "${ANDROID_SDK}/platform-tools")
#    message("adb: ${ADB}")
else()
#    set(BUILD_GMOCK OFF)
    set(INSTALL_GTEST OFF)

    include(FetchContent)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG        v1.14.0
        FIND_PACKAGE_ARGS NAMES GTest
        EXCLUDE_FROM_ALL
    )
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

    message("Fetching googletest...")
    FetchContent_MakeAvailable(googletest)
    include(GoogleTest)
endif()

set(PREVIEW_DEFAULT_TEST_CMAKE_ARGS)
get_cmake_property(CACHE_VARS CACHE_VARIABLES)
foreach(CACHE_VAR ${CACHE_VARS})
    get_property(CACHE_VAR_HELPSTRING CACHE ${CACHE_VAR} PROPERTY HELPSTRING)
    if(CACHE_VAR_HELPSTRING STREQUAL "No help, variable specified on the command line.")
        get_property(CACHE_VAR_TYPE CACHE ${CACHE_VAR} PROPERTY TYPE)
        if(CACHE_VAR_TYPE STREQUAL "UNINITIALIZED")
            set(CACHE_VAR_TYPE)
        else()
            set(CACHE_VAR_TYPE :${CACHE_VAR_TYPE})
        endif()
        list(APPEND PREVIEW_DEFAULT_TEST_CMAKE_ARGS "-D${CACHE_VAR}${CACHE_VAR_TYPE}=${${CACHE_VAR}}")
    endif()
endforeach()

# CMake somehow doesn't store CMAKE_TOOLCHAIN_FILE in cache for the first run
if (NOT ("${CMAKE_TOOLCHAIN_FILE}" STREQUAL ""))
    string(FIND "${PREVIEW_DEFAULT_TEST_CMAKE_ARGS}" "CMAKE_TOOLCHAIN_FILE" index)
    if (index EQUAL -1)
        list(APPEND PREVIEW_DEFAULT_TEST_CMAKE_ARGS "-DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}")
    endif ()
endif ()

message("PREVIEW_DEFAULT_TEST_CMAKE_ARGS: ${PREVIEW_DEFAULT_TEST_CMAKE_ARGS}")

# Create test targets per supported C++ version
function(preview_add_multi_version_test name_prefix test_src)
    add_custom_target(${name_prefix}_all)
    foreach(std ${std_list})
        set(target_name ${name_prefix}_${std})

        add_executable(${target_name} "${test_src}")
        set_property(TARGET ${target_name} PROPERTY CXX_STANDARD ${std})
        target_link_libraries(${target_name}
            GTest::gtest_main
            preview
        )
        add_dependencies(${name_prefix}_all ${target_name})
        
        if (NOT ANDROID)
            gtest_discover_tests(${target_name} DISCOVERY_TIMEOUT 60)
        endif ()
        
        if (ANDROID)
            #        add_custom_command(TARGET ${target_name} POST_BUILD
            #            COMMAND ${ADB} shell mkdir -p /data/local/tmp/${ANDROID_ABI}
            #            COMMAND ${ADB} push $<TARGET_FILE:${target_name}> /data/local/tmp/${ANDROID_ABI}/
            #            COMMAND ${ADB} shell \"export LD_LIBRARY_PATH=/data/local/tmp/${ANDROID_ABI}\; /data/local/tmp/${ANDROID_ABI}/${target_name}\")
        elseif (EMSCRIPTEN)
            target_link_libraries(${target_name} "-sEXIT_RUNTIME=1 -sALLOW_MEMORY_GROWTH=1 -Wno-pthreads-mem-growth -fexceptions")
            target_compile_options(${target_name}
                PUBLIC "-fexceptions"
            )
            gtest_discover_tests(${target_name} DISCOVERY_TIMEOUT 60)
        elseif (MSVC)
            target_compile_options(${target_name}
                PRIVATE "/bigobj"
                PRIVATE "/WX"
            )
            target_compile_definitions(${target_name}
                PRIVATE _CRT_SECURE_NO_WARNINGS)
        elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
            target_compile_options(${target_name}
                PRIVATE "-Wall"
                PRIVATE "-Werror"
                PRIVATE "-Wno-unused-local-typedef"
                PRIVATE "-Wno-unused-variable"
            )
        endif ()

        get_target_property(flag_list ${target_name} COMPILE_OPTIONS)
        string (REPLACE ";" " " flags "${flag_list}")
        get_target_property(THIS_BINARY_DIR ${target_name} BINARY_DIR)

        set(cmake_args)
        if (flags)
            list(APPEND cmake_args "-DCMAKE_CXX_FLAGS=${flags}")
        endif ()
        if (NOT ("${PREVIEW_DEFAULT_TEST_CMAKE_ARGS}" STREQUAL ""))
            list(APPEND cmake_args "${PREVIEW_DEFAULT_TEST_CMAKE_ARGS}")
        endif ()
        list(APPEND cmake_args "-DCMAKE_CXX_STANDARD=${std}")
        list(APPEND cmake_args "-DPREVIEW_TEST=0")

        # configure-only (to generate test-specific config.h)
        ExternalProject_Add(preview-configure-${target_name}
            SOURCE_DIR "${PREVIEW_BASE_DIR}"

            CMAKE_ARGS "${cmake_args}"
            DOWNLOAD_COMMAND ""
            BUILD_COMMAND ""
            INSTALL_COMMAND ""
            BUILD_BYPRODUCTS
                "<BINARY_DIR>/${PREVIEW_RELATIVE_GENERATED_INCLUDE_DIR}/preview/cmake_config.h"
                "<BINARY_DIR>/CMakeCache.txt"
        )
        add_dependencies(${target_name} preview-configure-${target_name})

        ExternalProject_Get_Property(preview-configure-${target_name} BINARY_DIR)
        target_include_directories(${target_name}
            PRIVATE "${BINARY_DIR}/${PREVIEW_RELATIVE_GENERATED_INCLUDE_DIR}")
    endforeach ()
endfunction()

# Create test targets
FILE(GLOB_RECURSE test_srcs "*.cc")
foreach(test_src ${test_srcs})
    get_filename_component(test_name "${test_src}" NAME_WE)
    preview_add_multi_version_test(${test_name} "${test_src}")
endforeach ()
