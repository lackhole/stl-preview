include(PreviewTest)

message("Configuring tests...")

if (NOT DEFINED CMAKE_CXX_STANDARD)
    set(min_cxx_version 14)
else ()
    set(min_cxx_version ${CMAKE_CXX_STANDARD})
endif ()

preview_latest_supported_cxx_version(max_cxx_version)

# Create test targets per supported C++ version
function(preview_add_multi_version_test name_prefix test_src)
    add_custom_target(${name_prefix}_all)
    foreach(std RANGE ${min_cxx_version} ${max_cxx_version} 3)
        preview_add_test(${name_prefix}_${std} "${test_src}" PROPERTIES LABELS "${name_prefix}_all")
        set_target_properties(${name_prefix}_${std} PROPERTIES CXX_STANDARD ${std})
        target_link_libraries(${name_prefix}_${std} preview)

        add_dependencies(${name_prefix}_all ${name_prefix}_${std})
    endforeach ()
endfunction()

# Create test targets
FILE(GLOB_RECURSE test_srcs "*.cc")
foreach(test_src ${test_srcs})
    file(RELATIVE_PATH test_src_rel "${CMAKE_CURRENT_LIST_DIR}" "${test_src}")
    file(TO_CMAKE_PATH test_src_rel "${test_src_rel}")
    string(REPLACE "/" "_" test_name ${test_src_rel})
    get_filename_component(test_name "${test_name}" NAME_WE)
    preview_add_multi_version_test(${test_name} "${test_src}")
endforeach ()
