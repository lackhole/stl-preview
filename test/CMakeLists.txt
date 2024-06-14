set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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
    include(FetchContent)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG        v1.14.0
        FIND_PACKAGE_ARGS NAMES GTest
    )
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

    message("Fetching googletest...")
    FetchContent_MakeAvailable(googletest)
    include(GoogleTest)
endif()

FILE(GLOB_RECURSE test_srcs "*.cc")
foreach(test_src ${test_srcs})
    get_filename_component(test_name "${test_src}" NAME_WE)
    add_executable(${test_name} "${test_src}")
    target_link_libraries(
        ${test_name}
        GTest::gtest_main
        preview
    )
    if (ANDROID)
#        add_custom_command(TARGET ${test_name} POST_BUILD
#            COMMAND ${ADB} shell mkdir -p /data/local/tmp/${ANDROID_ABI}
#            COMMAND ${ADB} push $<TARGET_FILE:${test_name}> /data/local/tmp/${ANDROID_ABI}/
#            COMMAND ${ADB} shell \"export LD_LIBRARY_PATH=/data/local/tmp/${ANDROID_ABI}\; /data/local/tmp/${ANDROID_ABI}/${test_name}\")
    else()
        if (EMSCRIPTEN)
            target_link_libraries(${test_name} "-sEXIT_RUNTIME=1 -sALLOW_MEMORY_GROWTH=1")
        elseif (MSVC)
            target_compile_options(${test_name} PRIVATE "/bigobj")
        endif()
        gtest_discover_tests(${test_name})
    endif()
endforeach ()
