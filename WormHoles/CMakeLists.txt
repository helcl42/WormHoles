project(WormHoles)
cmake_minimum_required(VERSION 3.0)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
SET(SOURCE_GROUP_DELIMITER "/")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(DCMAKE_CXX_EXTENSIONS OFF)

include_directories("${PROJECT_SOURCE_DIR}")
include_directories("./WormHoles")

FILE(GLOB WORM_HOLES_SRC_LIST "worm/*.h" "worm/*.cpp")
FILE(GLOB WORM_HOLES_INTERNAL_SRC_LIST "worm/detail/*.h" "worm/detail/*.cpp")

SOURCE_GROUP("worm" FILES ${WORM_HOLES_SRC_LIST})
SOURCE_GROUP("worm/detail" FILES ${WORM_HOLES_INTERNAL_SRC_LIST})

add_library(${PROJECT_NAME} INTERFACE)