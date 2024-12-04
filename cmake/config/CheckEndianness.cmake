set(AllTypes1Byte_Source
"int main() {
    static_asset(sizeof(char) == 1, \"\") &&
    static_asset(sizeof(short) == 1, \"\") &&
    static_asset(sizeof(int) == 1, \"\") &&
    static_asset(sizeof(long) == 1, \"\") &&
    static_asset(sizeof(long long) == 1, \"\") &&
    static_asset(sizeof(bool) == 1, \"\");
}")

check_cxx_source_compiles(AllTypes1Byte_Source PREVIEW_ARE_ALL_TYPES_1_BYTE)

set(PREVIEW_BIG_ENDIAN 0)
set(PREVIEW_LITTLE_ENDIAN 1)
set(PREVIEW_OTHER_ENDIAN 2)

file(WRITE ${CMAKE_BINARY_DIR}/CheckEndian.cpp
"#include <cstdint>
#include <fstream>

int main(int argc, char* argv[]) {
    const std::uint32_t val{ 0x01'23'45'67 };
    const std::uint8_t* ptr{ reinterpret_cast<const std::uint8_t*>(&val) };
    std::ofstream out{ argv[1] };

    if (ptr[0] == 0x01 && ptr[1] == 0x23 && ptr[2] == 0x45 && ptr[3] == 0x67) {
        out << ${PREVIEW_BIG_ENDIAN} << std::endl;
    } else if (ptr[0] == 0x67 && ptr[1] == 0x45 && ptr[2] == 0x23 && ptr[3] == 0x01) {
        out << ${PREVIEW_LITTLE_ENDIAN} << std::endl;
    } else {
        out << ${PREVIEW_OTHER_ENDIAN} << std::endl;
    }
}")

if (PREVIEW_ARE_ALL_TYPES_1_BYTE EQUAL 1)
    add_compile_definitions(PREVIEW_BIG_ENDIAN=0)
    add_compile_definitions(PREVIEW_LITTLE_ENDIAN=0)
    add_compile_definitions(PREVIEW_OTHER_ENDIAN=0)
else()
    add_compile_definitions(PREVIEW_BIG_ENDIAN=${PREVIEW_BIG_ENDIAN})
    add_compile_definitions(PREVIEW_LITTLE_ENDIAN=${PREVIEW_LITTLE_ENDIAN})
    add_compile_definitions(PREVIEW_OTHER_ENDIAN=${PREVIEW_OTHER_ENDIAN})
endif()

try_run(
    runres compileres ${CMAKE_BINARY_DIR}
    SOURCES ${CMAKE_BINARY_DIR}/CheckEndian.cpp
    ARGS ${CMAKE_BINARY_DIR}/endian.txt
)

file(READ ${CMAKE_BINARY_DIR}/endian.txt PREVIEW_ENDIAN)

if(PREVIEW_ENDIAN EQUAL ${PREVIEW_BIG_ENDIAN})
    message(STATUS "Big endian")
    add_compile_definitions(PREVIEW_ENDIAN=${PREVIEW_BIG_ENDIAN})
elseif(PREVIEW_ENDIAN EQUAL ${PREVIEW_LITTLE_ENDIAN})
    message(STATUS "Little endian")
    add_compile_definitions(PREVIEW_ENDIAN=${PREVIEW_LITTLE_ENDIAN})
else()
    message(STATUS "Other endian")
    add_compile_definitions(PREVIEW_ENDIAN=${PREVIEW_OTHER_ENDIAN})
endif()
