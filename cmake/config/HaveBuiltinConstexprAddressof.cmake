set(HaveBuiltinConstexprAddressof_Source "
constexpr int x = 0;
constexpr auto ptr = __builtin_addressof(x);
static_assert(ptr == &x, \"\");
int main() { return 0; }")

function(HaveBuiltinConstexprAddressof out)
    set(CMAKE_REQUIRED_FLAGS ${ARGN})
    check_cxx_source_compiles("${HaveBuiltinConstexprAddressof_Source}" ${out})
    set(${out} ${${out}} PARENT_SCOPE)
endfunction()
