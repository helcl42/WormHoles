cmake_minimum_required(VERSION 3.10)
project(WormHoles)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
SET(SOURCE_GROUP_DELIMITER "/")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(DCMAKE_CXX_EXTENSIONS OFF)

include_directories("${PROJECT_SOURCE_DIR}")
include_directories("./WormHoles")

file(GLOB WORM_HOLES_SRC_LIST 
	"worm/*.h" "worm/*.cpp"
	"worm/detail/*.h" "worm/detail/*.cpp"
)

add_library(${PROJECT_NAME} INTERFACE ${WORM_HOLES_SRC_LIST})