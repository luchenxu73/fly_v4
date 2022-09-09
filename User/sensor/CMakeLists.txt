cmake_minimum_required(VERSION 3.22)

include_directories(inc)
file(GLOB SENSOR_SRC ./src/*.c)

add_library(SENSOR_LIB ${SENSOR_SRC})