include(FindGoogleTest)

function(preview_add_test name source)
    add_executable(${name} "${source}")
    target_link_libraries(${name} GTest::gtest_main)
    add_test(NAME ${name} COMMAND ${name})

    if (NOT ANDROID)
        gtest_discover_tests(${name}
            DISCOVERY_TIMEOUT 60
            ${ARGN}
        )
    endif ()

    if (ANDROID)
        #        add_custom_command(TARGET ${name} POST_BUILD
        #            COMMAND ${ADB} shell mkdir -p /data/local/tmp/${ANDROID_ABI}
        #            COMMAND ${ADB} push $<TARGET_FILE:${name}> /data/local/tmp/${ANDROID_ABI}/
        #            COMMAND ${ADB} shell \"export LD_LIBRARY_PATH=/data/local/tmp/${ANDROID_ABI}\; /data/local/tmp/${ANDROID_ABI}/${name}\")
    elseif (EMSCRIPTEN)
        include(ProcessorCount)
        ProcessorCount(N)
        if(N EQUAL 0)
            set(N 2)
        endif()
        target_link_libraries(${name} "-sUSE_PTHREADS=1 -sPTHREAD_POOL_SIZE=${N} -sEXIT_RUNTIME=1 -sALLOW_MEMORY_GROWTH=1 -Wno-pthreads-mem-growth -fexceptions")
        target_compile_options(${name}
            PUBLIC "-fexceptions")
    elseif (MSVC)
        target_compile_options(${name}
            PRIVATE "/bigobj"
            PRIVATE "/WX")
        target_compile_definitions(${name}
            PRIVATE _CRT_SECURE_NO_WARNINGS)
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        target_compile_options(${name}
            PRIVATE "-Wall"
            PRIVATE "-Werror"
            PRIVATE "-Wno-unused-local-typedef"
            PRIVATE "-Wno-unused-variable"
        )
    endif ()
endfunction()
