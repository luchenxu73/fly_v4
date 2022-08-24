cmake_minimum_required(VERSION 3.22)

include_directories(./inc)
file(GLOB USER_SRC ./src/*.c)
#aux_source_directory(./src USER_SRC)

add_subdirectory(./drone_control)

add_library(USER_LIB ${USER_SRC})
