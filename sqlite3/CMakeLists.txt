cmake_minimum_required(VERSION 3.0)

set(CMAKE_C_FLAGS_TEST ${CMAKE_C_FLAGS_DEBUG})
set(CMAKE_C_FLAGS_FASTTEST ${CMAKE_C_FLAGS_RELEASE})

add_library(sqlite3 STATIC sqlite3.c sqlite3.h)

if (CMAKE_HOST_WIN32)
	target_compile_options(sqlite3 PUBLIC /MP /MDd)
endif ()

# vim: set ts=4 sw=4 ai tw=0 noet syntax=cmake :

