cmake_minimum_required(VERSION 3.22)

include_directories(./inc)
file(GLOB CONTROL_SRC ./src/*.c)

add_library(CONTROL_LIB ${CONTROL_SRC})