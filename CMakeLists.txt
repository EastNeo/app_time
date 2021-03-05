cmake_minimum_required(VERSION 3.5)

project(appointment)

set(tools /usr/local/TINA/toolchain)
SET(CMAKE_C_COMPILER ${tools}/bin/arm-openwrt-linux-gcc)
SET(CMAKE_CXX_COMPILER ${tools}/bin/arm-openwrt-linux-g++)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_INCLUDE_CURRENT_DIR on)

set(third_dir ${PROJECT_SOURCE_DIR}/third_party)

include_directories(
    ${third_dir}/bv_sqlite3
)

add_subdirectory(src)
add_subdirectory(${third_dir}/bv_sqlite3)
