include(CheckCXXSourceCompiles)
include(CheckCXXCompilerFlag)
include(CheckCXXSymbolExists)

include(config/HaveContiguousIteratorTag)
include(config/HaveStringView)

set(PREVIEW_CONFIG_INPUT_FILE "${CMAKE_CURRENT_LIST_DIR}/config.h.in")

function(RunConfiguration out_dir flags)
    HaveContiguousIteratorTag(PREVIEW_HAVE_CONTIGUOUS_ITERATOR_TAG ${flags})
    HaveStringView(PREVIEW_HAVE_STRING_VIEW ${flags})

    configure_file("${PREVIEW_CONFIG_INPUT_FILE}" "${out_dir}")
endfunction()
