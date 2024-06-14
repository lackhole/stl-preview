set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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

include(CTest)
include(GoogleTest)

FILE(GLOB_RECURSE test_srcs "*.cc")
foreach(test_src ${test_srcs})
    get_filename_component(test_name "${test_src}" NAME_WE)
    add_executable(${test_name} "${test_src}")
    target_link_libraries(
        ${test_name}
        GTest::gtest_main
        preview
    )
    gtest_discover_tests(${test_name})
endforeach ()
