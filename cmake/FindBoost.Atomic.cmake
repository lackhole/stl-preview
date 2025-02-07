if (PREVIEW_USE_SYSTEM_BOOST)
    if ("${PREVIEW_BOOST_PATH}" STREQUAL "")
        find_package(Boost REQUIRED COMPONENTS atomic)
    else ()
        find_package(Boost REQUIRED COMPONENTS atomic PATHS "${PREVIEW_BOOST_PATH}")
    endif ()
else()
    include(FetchContent)

    FetchContent_Declare(boost_config
        GIT_REPOSITORY https://github.com/boostorg/config
        GIT_TAG 5734e160e08b8df898c7f747000f27a3aafb7b2b
    )
    FetchContent_Declare(boost_predef
        GIT_REPOSITORY https://github.com/boostorg/predef
        GIT_TAG boost-1.86.0
    )
    FetchContent_Declare(boost_preprocessor
        GIT_REPOSITORY https://github.com/boostorg/preprocessor
        GIT_TAG boost-1.86.0
    )
    FetchContent_MakeAvailable(boost_config boost_predef boost_preprocessor)

    add_subdirectory("${PREVIEW_THIRD_PARTY_DIR}/boost_1_86_0/atomic" "${CMAKE_CURRENT_BINARY_DIR}/boost")
endif ()
