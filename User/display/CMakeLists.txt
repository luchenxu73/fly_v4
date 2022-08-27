cmake_minimum_required(VERSION 3.22)

include_directories(inc)

file(GLOB DISPLAY_SRC src/*.c)

add_library(DISPLAY_LIB ${DISPLAY_SRC})