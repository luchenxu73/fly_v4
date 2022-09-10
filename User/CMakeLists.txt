cmake_minimum_required(VERSION 3.22)

include_directories(./inc)
include_directories(drone_control/inc)
include_directories(display/inc)
include_directories(tasks/inc)
include_directories(sensor/inc)
include_directories(fusion/inc)

file(GLOB USER_SRC ./src/*.c)
#aux_source_directory(./src USER_SRC)


add_subdirectory(display)
add_subdirectory(drone_control)
add_subdirectory(sensor)
add_subdirectory(fusion)

#add_subdirectory(tasks)

add_library(USER_LIB ${USER_SRC})
