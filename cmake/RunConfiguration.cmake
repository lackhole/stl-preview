include(CheckCXXSourceCompiles)
include(CheckCXXCompilerFlag)
include(CheckCXXSymbolExists)

include(config/HaveBuiltinBitCast)
include(config/HaveBuiltinConstexprAddressof)
include(config/HaveContiguousIteratorTag)
include(config/HaveStringView)

set(PREVIEW_CONFIG_INPUT_FILE "${PREVIEW_INCLUDE_DIR}/preview/cmake_config.h.in")

function(RunConfiguration out_dir flags)
    HaveBuiltinBitCast(PREVIEW_HAVE_BUILTIN_BIT_CAST ${flags})
    HaveBuiltinConstexprAddressof(PREVIEW_HAVE_BUILTIN_CONSTEXPR_ADDRESSOF ${flags})

    HaveContiguousIteratorTag(PREVIEW_HAVE_CONTIGUOUS_ITERATOR_TAG ${flags})
    HaveStringView(PREVIEW_HAVE_STRING_VIEW ${flags})

    configure_file("${PREVIEW_CONFIG_INPUT_FILE}" "${out_dir}")
endfunction()
