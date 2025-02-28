set(HaveStringView_Source "
#include <string>
#include <string_view>

using namespace std::literals;

int main() {
    std::string s;
    std::basic_string_view<char> bsv;
    std::string_view sv = s;

    auto svl = \"foobar\"sv;

    return 0;
}")

function(HaveStringView out)
    set(CMAKE_REQUIRED_FLAGS ${ARGN})
    PREVIEW_CHECK_CXX_SOURCE_COMPILES_UNTIL_CXX(17 "${HaveStringView_Source}" ${out})
    set(${out} ${${out}} PARENT_SCOPE)
endfunction()
