FILE(GLOB_RECURSE cpp_srcs "./*.cpp")
FILE(GLOB_RECURSE c_srcs "./*.c")
FILE(GLOB_RECURSE all_hrds "./*.h")

set(CMAKE_CXX_FLAGS_RELEASE "-03 -s")

link_directories(${PROJECT_SOURCE_DIR}/lib)

add_executable(main ${cpp_srcs} ${cpp_srcs} ${all_hrds} db/sqlite3_handle.h db/sqlite3_handle.cpp)
target_link_libraries(main PUBLIC bvsqlite pthread dl)
