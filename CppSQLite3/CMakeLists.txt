cmake_minimum_required(VERSION 3.0)

include_directories(${CMAKE_SOURCE_DIR}/sqlite3)

add_library(CppSQLite3 STATIC CppSQLite3.h CppSQLite3.cpp LICENSE)

target_link_libraries(CppSQLite3 sqlite3)

if (CMAKE_HOST_WIN32)
	target_compile_options(CppSQLite3 PUBLIC /MP /MDd)
	target_compile_options(CppSQLite3 PUBLIC $<$<CONFIG:Debug>:/Zi /Od>)
	target_compile_options(CppSQLite3 PUBLIC $<$<CONFIG:Release>:/O2>)
	target_compile_options(CppSQLite3 PUBLIC $<$<CONFIG:Test>:/Zi /Od>)
	target_compile_options(CppSQLite3 PUBLIC $<$<CONFIG:FastTest>:/O2>)
endif ()

# vim: set ts=4 sw=4 ai tw=0 noet syntax=cmake :
