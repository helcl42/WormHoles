cmake_minimum_required(VERSION 3.10)
project(SimpleExample)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
SET(SOURCE_GROUP_DELIMITER "/")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(DCMAKE_CXX_EXTENSIONS OFF)

include_directories("${PROJECT_SOURCE_DIR}")
include_directories("../WormHoles/")

file(GLOB SIMPLE_EXAMPLE_SRC_LIST 
	"*.h" "*.cpp"
)

add_executable(${PROJECT_NAME} ${SIMPLE_EXAMPLE_SRC_LIST})

# testing
enable_testing()

add_test(NAME SimpleDefaultTest COMMAND $<TARGET_FILE:${PROJECT_NAME}>)




