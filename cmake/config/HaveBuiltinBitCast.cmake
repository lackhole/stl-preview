set(HaveBuiltinBitCast_Source "
union U {
  int i;
  char c[sizeof(int)];
};

int main() {
  U u{10};
  int x = __builtin_bit_cast(int, u.c);
  return 0;
}")

function(HaveBuiltinBitCast out)
    set(CMAKE_REQUIRED_FLAGS ${ARGN})
    check_cxx_source_compiles("${HaveBuiltinBitCast_Source}" ${out})
    set(${out} ${${out}} PARENT_SCOPE)
endfunction()
