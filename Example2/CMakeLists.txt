cmake_minimum_required(VERSION 3.10)
project(Example2)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
SET(SOURCE_GROUP_DELIMITER "/")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(DCMAKE_CXX_EXTENSIONS OFF)

include_directories("../WormHoles/")

file(GLOB COMPLEX_EXAMPLE_SRC_LIST 
	"*.h" "*.cpp"
)

add_executable(${PROJECT_NAME} ${COMPLEX_EXAMPLE_SRC_LIST})

# testing
enable_testing()

add_test(NAME ComplexDefaultTest COMMAND $<TARGET_FILE:${PROJECT_NAME}>)
