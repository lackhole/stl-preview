include(CheckCXXSourceCompiles)
include(CheckCXXCompilerFlag)
include(CheckCXXSymbolExists)
include(CheckCXXSourceRuns)

include(config/HaveBuiltinBitCast)
include(config/HaveBuiltinConstexprAddressof)
include(config/HaveContiguousIteratorTag)
include(config/HaveCXX20Atomic)
include(config/HaveStringView)

set(PREVIEW_CONFIG_INPUT_FILE "${PREVIEW_INCLUDE_DIR}/preview/cmake_config.h.in")

macro(PREVIEW_RUN_CONFIGURATION_TEST flags)
    message("Test flags: ${flags}")

    HaveBuiltinBitCast(PREVIEW_HAVE_BUILTIN_BIT_CAST ${flags})
    HaveBuiltinConstexprAddressof(PREVIEW_HAVE_BUILTIN_CONSTEXPR_ADDRESSOF ${flags})

    HaveContiguousIteratorTag(PREVIEW_HAVE_CONTIGUOUS_ITERATOR_TAG ${flags})
    HaveStringView(PREVIEW_HAVE_STRING_VIEW ${flags})

    HaveCXX20Atomic(PREVIEW_HAVE_CXX20_ATOMIC ${flags})
endmacro()

macro(PREVIEW_CONFIGURE_FILE out_dir)
    configure_file("${PREVIEW_CONFIG_INPUT_FILE}" "${out_dir}")
endmacro()
