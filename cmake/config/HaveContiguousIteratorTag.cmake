set(HaveContiguousIteratorTag_Source "
#include <iterator>

using iterator_category = std::contiguous_iterator_tag;

int main() { return 0; }")

function(HaveContiguousIteratorTag out)
    set(CMAKE_REQUIRED_FLAGS ${ARGN})
    check_cxx_source_compiles("${HaveContiguousIteratorTag_Source}" ${out})
    set(${out} ${${out}} PARENT_SCOPE)
endfunction()
