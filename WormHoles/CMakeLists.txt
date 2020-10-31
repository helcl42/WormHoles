project(WormHoles)
cmake_minimum_required(VERSION 2.8)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
SET(SOURCE_GROUP_DELIMITER "/")

include_directories("${PROJECT_SOURCE_DIR}")
include_directories("./WormHoles")

FILE(GLOB WORM_HOLES_SRC_LIST "worm/*.h" "worm/*.cpp")
FILE(GLOB WORM_HOLES_INTERNAL_SRC_LIST "worm/internal/*.h" "worm/internal/*.cpp")

SOURCE_GROUP("worm" FILES ${WORM_HOLES_SRC_LIST})
SOURCE_GROUP("worm/internal" FILES ${WORM_HOLES_INTERNAL_SRC_LIST})

add_library(${PROJECT_NAME} INTERFACE)

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
endif()

set(CMAKE_CXX_STANDARD 17)
target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_17)