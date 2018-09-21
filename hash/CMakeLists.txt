cmake_minimum_required(VERSION 2.4)
project(demo)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -g")

set(SOURCE_FILES demo.c equal.c hash.c hashcode.c list.c)

add_executable(demo ${SOURCE_FILES})
