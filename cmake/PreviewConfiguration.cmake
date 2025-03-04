include(CheckCXXSourceCompiles)
include(CheckCXXCompilerFlag)
include(CheckCXXSymbolExists)
include(CheckCXXSourceRuns)

include(PreviewUtils)
include(config/HaveBuiltinBitCast)
include(config/HaveBuiltinConstexprAddressof)
include(config/HaveAtomicWaitAndNotify)
include(config/HaveStringView)

if ("${PREVIEW_INCLUDE_DIR}" STREQUAL "")
    message(FATAL_ERROR "PREVIEW_INCLUDE_DIR must be set")
endif ()

set(PREVIEW_CONFIG_INPUT_FILE "${PREVIEW_INCLUDE_DIR}/preview/cmake_config.h.in")

# Override CMAKE_CXX_STANDARD to the specified value.
# Must be restored by invoking PREVIEW_RESTORE_CXX_STANDARD
macro(PREVIEW_OVERRIDE_CXX_STANDARD version)
    if (DEFINED _PREVIEW_CMAKE_CXX_STANDARD_OVERRIDED)
        message(FATAL_ERROR "PREVIEW_OVERRIDE_CXX_STANDARD cannot be called twice")
    endif ()
    set(_PREVIEW_CMAKE_CXX_STANDARD_BACKUP ${CMAKE_CXX_STANDARD})
    set(_PREVIEW_CMAKE_CXX_STANDARD_OVERRIDED 1)
    set(CMAKE_CXX_STANDARD ${version})
endmacro()

# Restore CMAKE_CXX_STANDARD to the original value before invoking PREVIEW_OVERRIDE_CXX_STANDARD
macro(PREVIEW_RESTORE_CXX_STANDARD)
    if (NOT _PREVIEW_CMAKE_CXX_STANDARD_OVERRIDED)
        message(FATAL_ERROR "PREVIEW_OVERRIDE_CXX_STANDARD must be called after PREVIEW_DISABLE_CXX_STANDARD")
    endif ()
    set(CMAKE_CXX_STANDARD ${_PREVIEW_CMAKE_CXX_STANDARD_BACKUP})
    unset(_PREVIEW_CMAKE_CXX_STANDARD_BACKUP)
    unset(_PREVIEW_CMAKE_CXX_STANDARD_OVERRIDED)
endmacro()

function(preview_check_cplusplus_is_greater_or_equal cplusplus out)
    if (MSVC)
        set(CMAKE_REQUIRED_FLAGS "${CMAKE_REQUIRED_FLAGS} /Zc:__cplusplus")
    endif()

    set(source "
#if __cplusplus < ${cplusplus}
#error 1
#endif
int main() { return 0; }")
    check_cxx_source_compiles("${source}" ${out})
endfunction()

# Check which C++ standards are supported by the compiler.
# Sets PREVIEW_COMPILER_SUPPORT_CXX<VERSION> variables.
function(preview_run_cxx_standard_support_check)
    PREVIEW_OVERRIDE_CXX_STANDARD(14)
        preview_check_cplusplus_is_greater_or_equal("201402L" PREVIEW_COMPILER_SUPPORT_CXX14)
    PREVIEW_RESTORE_CXX_STANDARD()
    if (NOT PREVIEW_COMPILER_SUPPORT_CXX14)
        message(FATAL_ERROR "Compiler must support C++14")
    endif ()

    PREVIEW_OVERRIDE_CXX_STANDARD(17)
        preview_check_cplusplus_is_greater_or_equal("201703L" PREVIEW_COMPILER_SUPPORT_CXX17)
    PREVIEW_RESTORE_CXX_STANDARD()
    if (NOT PREVIEW_COMPILER_SUPPORT_CXX17)
        return()
    endif ()

    PREVIEW_OVERRIDE_CXX_STANDARD(20)
        preview_check_cplusplus_is_greater_or_equal("202002L" PREVIEW_COMPILER_SUPPORT_CXX20)
    PREVIEW_RESTORE_CXX_STANDARD()
    if (NOT PREVIEW_COMPILER_SUPPORT_CXX20)
        return()
    endif ()

    PREVIEW_OVERRIDE_CXX_STANDARD(23)
        preview_check_cplusplus_is_greater_or_equal("202302L" PREVIEW_COMPILER_SUPPORT_CXX23)
    PREVIEW_RESTORE_CXX_STANDARD()
    if (NOT PREVIEW_COMPILER_SUPPORT_CXX23)
        return()
    endif ()
endfunction()

function(preview_latest_supported_cxx_version out)
    if (PREVIEW_COMPILER_SUPPORT_CXX23)
        set(${out} 23 PARENT_SCOPE)
    elseif (PREVIEW_COMPILER_SUPPORT_CXX20)
        set(${out} 20 PARENT_SCOPE)
    elseif (PREVIEW_COMPILER_SUPPORT_CXX17)
        set(${out} 17 PARENT_SCOPE)
    elseif (PREVIEW_COMPILER_SUPPORT_CXX14)
        set(${out} 14 PARENT_SCOPE)
    else ()
        message(FATAL_ERROR "Execute preview_run_cxx_standard_support_check first")
    endif ()
endfunction()

# Check if the specified C++ source code compiles for the specified C++ version.
macro(PREVIEW_CHECK_CXX_SOURCE_COMPILES_FOR_CXX cxx_version source out)
    PREVIEW_OVERRIDE_CXX_STANDARD(${cxx_version})
        preview_check_cxx_source_compiles("${source}" ${out} ${ARGN})
    PREVIEW_RESTORE_CXX_STANDARD()
endmacro()

# Check if the specified C++ source code compiles for C++ [since, until] versions.
# Sets ${out}_SINCE_CXX<VERSION> variables.
macro(PREVIEW_CHECK_CXX_SOURCE_COMPILES_SINCE_UNTIL_CXX since until source out)
    # TODO: Fail fast
    foreach (cxx_version RANGE ${since} ${until} 3)
        PREVIEW_CHECK_CXX_SOURCE_COMPILES_FOR_CXX(${cxx_version} "${source}" ${out}_SINCE_CXX${cxx_version} ${ARGN})
    endforeach ()
endmacro()

# Check if the specified C++ source code compiles until the C++ version reaches the specified version.
# Sets ${out}_SINCE_CXX<VERSION> variables.
macro(PREVIEW_CHECK_CXX_SOURCE_COMPILES_UNTIL_CXX cxx_version source out)
    PREVIEW_CHECK_CXX_SOURCE_COMPILES_SINCE_UNTIL_CXX(14 ${cxx_version} "${source}" ${out} ${ARGN})
endmacro()

macro(PREVIEW_RUN_COMPILE_CHECKS)
    message("Configuration flags: ${flags}")

    HaveBuiltinBitCast(PREVIEW_HAVE_BUILTIN_BIT_CAST ${flags})
    HaveBuiltinConstexprAddressof(PREVIEW_HAVE_BUILTIN_CONSTEXPR_ADDRESSOF ${flags})

    HaveStringView(PREVIEW_HAVE_STRING_VIEW ${flags})
    HaveAtomicWaitAndNotify(PREVIEW_HAVE_ATOMIC_WAIT_AND_NOTIFY ${flags})
endmacro()

macro(PREVIEW_CONFIGURE_FILE out_dir)
    configure_file("${PREVIEW_CONFIG_INPUT_FILE}" "${out_dir}")
endmacro()

preview_run_cxx_standard_support_check()
