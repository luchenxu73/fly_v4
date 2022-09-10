cmake_minimum_required(VERSION 3.22)

include_directories(inc)
file(GLOB_RECURSE FUSION_SRC "src/*.c")

add_library(FUSION_LIB ${FUSION_SRC})