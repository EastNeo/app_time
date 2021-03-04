cmake_minimum_required(VERSION 3.5)

set(target "bvsqlite")

file(GLOB_RECURSE cpp_srcs "./*.cpp")
file(GLOB_RECURSE c_srcs "./*.c")
file(GLOB_RECURSE all_hrds "./*.h")

set(CMAKE_CXX_FLAGS_RELEASE "-03 -s -pthread -ldl")

add_library(${target} ${cpp_srcs} ${c_srcs} ${all_hrds})
