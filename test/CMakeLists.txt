include(PreviewTest)

message("Configuring tests...")

preview_latest_supported_cxx_version(max_cxx_version)

# Create test targets per supported C++ version
function(preview_add_multi_version_test name_prefix test_src test_group)
    if (NOT TARGET ${test_group})
        add_custom_target(${test_group})
    endif ()
    foreach(std RANGE 14 ${max_cxx_version} 3)
        preview_add_test(${name_prefix}_${std} "${test_src}")
        set_target_properties(${name_prefix}_${std} PROPERTIES CXX_STANDARD ${std})
        target_link_libraries(${name_prefix}_${std} preview)

        add_dependencies(${test_group} ${name_prefix}_${std})
    endforeach ()
endfunction()

# Create test targets
FILE(GLOB_RECURSE test_srcs "*.cc")
foreach(test_src ${test_srcs})

    get_filename_component(test_dir "${test_src}" DIRECTORY)
    file(RELATIVE_PATH test_dir "${CMAKE_CURRENT_LIST_DIR}" "${test_dir}")

    if ("${test_dir}" STREQUAL "")
        get_filename_component(test_name "${test_src}" NAME_WLE)
        set(test_group "${test_name}_all")
    else ()
        get_filename_component(test_name "${test_src}" NAME_WLE)
        string(REPLACE "/" "_" test_name "${test_dir}_${test_name}")
        string(REPLACE "\\" "_" test_name "${test_name}")

        string(REGEX MATCH "^[^/\\]+" base_dir "${test_dir}")
        set(test_group "${base_dir}_all")
        string(REPLACE "/" "_" test_group "${test_group}")
        string(REPLACE "\\" "_" test_group "${test_group}")
    endif ()

#    file(RELATIVE_PATH test_src_rel "${CMAKE_CURRENT_LIST_DIR}" "${test_src}")
#    file(TO_CMAKE_PATH test_src_rel "${test_src_rel}")
#    string(REPLACE "/" "_" test_name ${test_src_rel})
#    get_filename_component(test_name "${test_name}" NAME_WE)
    preview_add_multi_version_test(${test_name} "${test_src}" "${test_group}")
endforeach ()
