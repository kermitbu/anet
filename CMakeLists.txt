cmake_minimum_required(VERSION 2.4)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -g -w")

set(SOURCE_FILES ae.c anet.c protocol.c buffer.c ae_event_func_table.c)

add_library(anet server.c ${SOURCE_FILES} ${HASH_SOURCE_FILES})