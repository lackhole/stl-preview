include(FetchContent)

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
