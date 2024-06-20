# Check maximum C++ version that compiler supports
include(CheckCXXSourceCompiles)
include(CheckCXXCompilerFlag)

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


# Create test targets per supported C++ version
function(preview_add_multi_version_test name_prefix test_src)
    add_custom_target(${name_prefix}_all)
    foreach(std ${std_list})
        add_executable(${name_prefix}_${std} "${test_src}")
        set_property(TARGET ${name_prefix}_${std} PROPERTY CXX_STANDARD ${std})
        target_link_libraries(
            ${name_prefix}_${std}
            GTest::gtest_main
            preview
        )
        add_dependencies(${name_prefix}_all ${name_prefix}_${std})

        if (ANDROID)
            #        add_custom_command(TARGET ${name_prefix}_${std} POST_BUILD
            #            COMMAND ${ADB} shell mkdir -p /data/local/tmp/${ANDROID_ABI}
            #            COMMAND ${ADB} push $<TARGET_FILE:${name_prefix}_${std}> /data/local/tmp/${ANDROID_ABI}/
            #            COMMAND ${ADB} shell \"export LD_LIBRARY_PATH=/data/local/tmp/${ANDROID_ABI}\; /data/local/tmp/${ANDROID_ABI}/${name_prefix}_${std}\")
        else()
            if (EMSCRIPTEN)
                target_link_libraries(${name_prefix}_${std} "-sEXIT_RUNTIME=1 -sALLOW_MEMORY_GROWTH=1 -Wno-pthreads-mem-growth -fexceptions")
            endif()
            gtest_discover_tests(${name_prefix}_${std} DISCOVERY_TIMEOUT 60)
        endif()

        if (EMSCRIPTEN)
            target_compile_options(${name_prefix}_${std}
                PUBLIC "-fexceptions"
            )
        elseif (MSVC)
            target_compile_options(${name_prefix}_${std}
                PRIVATE "/bigobj"
                PRIVATE "/WX")
        elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
            target_compile_options(${name_prefix}_${std}
                PRIVATE "-Wall"
                PRIVATE "-Werror"
                PRIVATE "-Wno-unused-local-typedef")
        endif ()
    endforeach ()
endfunction()

# Create test targets
FILE(GLOB_RECURSE test_srcs "*.cc")
foreach(test_src ${test_srcs})
    get_filename_component(test_name "${test_src}" NAME_WE)
    preview_add_multi_version_test(${test_name} "${test_src}")
endforeach ()
