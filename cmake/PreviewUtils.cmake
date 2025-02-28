# Got from: https://github.com/opencv/opencv/blob/ff18c9cc7904e878b10273265196e0238490e692/cmake/OpenCVUtils.cmake#L723

# Provides an option that the user can optionally select.
# Can accept condition to control when option is available for user.
# Usage:
#   option(<option_variable>
#          "help string describing the option"
#          <initial value or boolean expression>
#          [VISIBLE_IF <condition>]
#          [VERIFY <condition>])
macro(PREVIEW_OPTION variable description value)
    set(__value ${value})
    set(__condition "")
    set(__verification)
    set(__varname "__value")
    foreach(arg ${ARGN})
        if(arg STREQUAL "IF" OR arg STREQUAL "if" OR arg STREQUAL "VISIBLE_IF")
            set(__varname "__condition")
        elseif(arg STREQUAL "VERIFY")
            set(__varname "__verification")
        else()
            list(APPEND ${__varname} ${arg})
        endif()
    endforeach()
    unset(__varname)
    if(__condition STREQUAL "")
        set(__condition 2 GREATER 1)
    endif()

    if(${__condition})
        if(__value MATCHES ";")
            if(${__value})
                option(${variable} "${description}" ON)
            else()
                option(${variable} "${description}" OFF)
            endif()
        elseif(DEFINED ${__value})
            if(${__value})
                option(${variable} "${description}" ON)
            else()
                option(${variable} "${description}" OFF)
            endif()
        else()
            option(${variable} "${description}" ${__value})
        endif()
    else()
        if(DEFINED ${variable} AND "${${variable}}"  # emit warnings about turned ON options only.
                AND NOT (PREVIEW_HIDE_WARNING_UNSUPPORTED_OPTION OR "$ENV{PREVIEW_HIDE_WARNING_UNSUPPORTED_OPTION}")
        )
            message(WARNING "Unexpected option: ${variable} (=${${variable}})\nCondition: IF (${__condition})")
        endif()
        if(PREVIEW_UNSET_UNSUPPORTED_OPTION)
            unset(${variable} CACHE)
        endif()
    endif()
    if(__verification)
        set(PREVIEW_VERIFY_${variable} "${__verification}") # variable containing condition to verify
        list(APPEND PREVIEW_VERIFICATIONS "${variable}") # list of variable names (WITH_XXX;WITH_YYY;...)
    endif()
    unset(__condition)
    unset(__value)
endmacro()

# Check that each variable stored in PREVIEW_VERIFICATIONS list
# is consistent with actual detection result (stored as condition in PREVIEW_VERIFY_...) variables
function(preview_verify_config)
    set(broken_options)
    foreach(var ${PREVIEW_VERIFICATIONS})
        set(evaluated FALSE)
        if(${PREVIEW_VERIFY_${var}})
            set(evaluated TRUE)
        endif()
        #        status("Verifying ${var}=${${var}} => '${PREVIEW_VERIFY_${var}}'=${evaluated}")
        if (${var} AND NOT evaluated)
            list(APPEND broken_options ${var})
            message(WARNING
                    "Option ${var} is enabled but corresponding dependency "
                    "have not been found: \"${PREVIEW_VERIFY_${var}}\" is FALSE")
        elseif(NOT ${var} AND evaluated)
            list(APPEND broken_options ${var})
            message(WARNING
                    "Option ${var} is disabled or unset but corresponding dependency "
                    "have been explicitly turned on: \"${PREVIEW_VERIFY_${var}}\" is TRUE")
        endif()
    endforeach()
    if(broken_options)
        string(REPLACE ";" "\n" broken_options "${broken_options}")
        message(FATAL_ERROR
                "Some dependencies have not been found or have been forced, "
                "unset ENABLE_CONFIG_VERIFICATION option to ignore these failures "
                "or change following options:\n${broken_options}")
    endif()
endfunction()

function(preview_target_compile_option_if_possible target flag)
    check_cxx_compiler_flag("${flag}" CHECK_CXX_FLAG_${flag})
    if (CHECK_CXX_FLAG_${flag})
        target_compile_options(${target} PRIVATE "${flag}")
    endif()
endfunction()

function(preview_check_cxx_source_compiles code out)
    set(CMAKE_REQUIRED_FLAGS ${ARGN})
    check_cxx_source_compiles("${code}" ${out})
    if ("${${out}}" STREQUAL "")
        set(${out} 0) # Workaround: CMake sometimes sets empty string instead of FALSE
    endif ()
    set(${out} ${${out}} PARENT_SCOPE)
endfunction()

function(preview_get_cmake_args out)
    get_cmake_property(vars CACHE_VARIABLES)
    set(args)
    foreach(var ${vars})
        get_property(currentHelpString CACHE "${var}" PROPERTY HELPSTRING)
        if("${currentHelpString}" MATCHES "No help, variable specified on the command line." OR "${currentHelpString}" STREQUAL "")
            get_property(var_type CACHE ${var} PROPERTY TYPE)
            if(var_type STREQUAL "UNINITIALIZED")
                set(var_type)
            else()
                set(var_type :${var_type})
            endif()
            list(APPEND args "-D${var}${var_type}=${${var}}")
        endif()
    endforeach()
    set(${out} "${args}" PARENT_SCOPE)
endfunction()
