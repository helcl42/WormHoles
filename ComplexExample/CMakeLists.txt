project(ComplexExample)
cmake_minimum_required(VERSION 3.0)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
SET(SOURCE_GROUP_DELIMITER "/")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(DCMAKE_CXX_EXTENSIONS OFF)

include_directories("${PROJECT_SOURCE_DIR}")
include_directories("../WormHoles/")

FILE(GLOB COMPLEX_EXAMPLE_SRC_LIST "*.h" "*.cpp")

SOURCE_GROUP("" FILES ${COMPLEX_EXAMPLE_SRC_LIST})

add_executable(${PROJECT_NAME}
	${COMPLEX_EXAMPLE_SRC_LIST}
)





