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

    if (ptr[0] == 0x01 && ptr[1] == 0x23 && ptr[2] == 0x45 && ptr[3] == 0x67) {
        return ${PREVIEW_BIG_ENDIAN};
    } else if (ptr[0] == 0x67 && ptr[1] == 0x45 && ptr[2] == 0x23 && ptr[3] == 0x01) {
        return ${PREVIEW_LITTLE_ENDIAN};
    } else {
        return ${PREVIEW_OTHER_ENDIAN};
    }
}")

if (PREVIEW_ARE_ALL_TYPES_1_BYTE EQUAL 1)
    set(PREVIEW_BIG_ENDIAN 0)
    set(PREVIEW_LITTLE_ENDIAN 0)
    set(PREVIEW_NATIVE_ENDIAN 0)
else()
    if(DEFINED PREVIEW_ENDIAN)
        if(PREVIEW_ENDIAN STREQUAL "BIG")
        message(STATUS "Big endian (forced by config)")
            set(PREVIEW_NATIVE_ENDIAN PREVIEW_BIG_ENDIAN)
        elseif(PREVIEW_ENDIAN STREQUAL "LITTLE")
            set(PREVIEW_NATIVE_ENDIAN PREVIEW_LITTLE_ENDIAN)
            message(STATUS "Little endian (forced by config)")
        elseif(PREVIEW_ENDIAN STREQUAL "OTHER")
            set(PREVIEW_NATIVE_ENDIAN PREVIEW_OTHER_ENDIAN)
            message(STATUS "Other endian (forced by config)")
        else()
            message(FATAL_ERROR "PREVIEW_ENDIAN is '${PREVIEW_ENDIAN}', but must be either BIG, LITTLE or OTHER.")
        endif()
    else()
        try_run(
            PREVIEW_ENDIAN COMPILE_RES ${CMAKE_BINARY_DIR}
            SOURCES ${CMAKE_BINARY_DIR}/CheckEndian.cpp
            ARGS ${CMAKE_BINARY_DIR}/endian.txt
        )
        if(NOT COMPILE_RES)
            message(FATAL_ERROR "Internal error - Cannot build endian check")
        endif()

        if(PREVIEW_ENDIAN EQUAL ${PREVIEW_BIG_ENDIAN})
            message(STATUS "Big endian (detected)")
            set(PREVIEW_NATIVE_ENDIAN ${PREVIEW_BIG_ENDIAN})
        elseif(PREVIEW_ENDIAN EQUAL ${PREVIEW_LITTLE_ENDIAN})
            message(STATUS "Little endian (detected)")
            set(PREVIEW_NATIVE_ENDIAN ${PREVIEW_LITTLE_ENDIAN})
        else()
            message(STATUS "Other endian (detected)")
            set(PREVIEW_NATIVE_ENDIAN ${PREVIEW_OTHER_ENDIAN})
        endif()
    endif()
endif()
