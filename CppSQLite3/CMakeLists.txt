cmake_minimum_required(VERSION 3.0)

include_directories(${CMAKE_SOURCE_DIR}/sqlite3)

add_library(CppSQLite3 STATIC CppSQLite3.h CppSQLite3.cpp LICENSE)

target_link_libraries(CppSQLite3 sqlite3)

# vim: set ts=4 sw=4 ai tw=0 noet syntax=cmake :

