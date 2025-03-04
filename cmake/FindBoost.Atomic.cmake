if (PREVIEW_USE_SYSTEM_BOOST)
    if ("${PREVIEW_BOOST_PATH}" STREQUAL "")
        find_package(Boost REQUIRED COMPONENTS atomic)
    else ()
        find_package(Boost REQUIRED COMPONENTS atomic PATHS "${PREVIEW_BOOST_PATH}")
    endif ()
else()
    include(FetchContent)

    set(BOOST_INCLUDE_LIBRARIES atomic)
    set(BOOST_ENABLE_CMAKE ON)

    # TODO: Add mirror URL
    # TODO: Download necessary libraries only
    message("Downloading Boost...")
    FetchContent_Declare(Boost
        URL https://github.com/boostorg/boost/releases/download/boost-1.87.0/boost-1.87.0-cmake.zip
        URL_HASH SHA256=03530DEC778BC1B85B070F0B077F3B01FD417133509BB19FE7C142E47777A87B

        USES_TERMINAL_DOWNLOAD TRUE
        USES_TERMINAL_CONFIGURE TRUE
        USES_TERMINAL_BUILD TRUE
    )
    if (EMSCRIPTEN)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread" CACHE INTERNAL "")
    endif ()
    if (CMAKE_VERSION VERSION_LESS 3.14)
        FetchContent_GetProperties(Boost)
        if (NOT Boost_POPULATED)
            FetchContent_Populate(Boost)
        endif ()
    else ()
        FetchContent_MakeAvailable(Boost)
    endif ()
endif ()
