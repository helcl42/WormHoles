project(ComplexExample)
cmake_minimum_required(VERSION 2.8)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
SET(SOURCE_GROUP_DELIMITER "/")

include_directories("${PROJECT_SOURCE_DIR}")
include_directories("../WormHoles/")

FILE(GLOB COMPLEX_EXAMPLE_SRC_LIST "*.h" "*.cpp")

SOURCE_GROUP("" FILES ${COMPLEX_EXAMPLE_SRC_LIST})

add_executable(${PROJECT_NAME}
	${COMPLEX_EXAMPLE_SRC_LIST}
)

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
	TARGET_LINK_LIBRARIES(${PROJECT_NAME} pthread)
endif()

set(CMAKE_CXX_STANDARD 17)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)





