project(WormHoles)
cmake_minimum_required(VERSION 2.8)

include_directories("${PROJECT_SOURCE_DIR}")
include_directories("./WormHoles")

FILE(GLOB SRC_LIST "*.h" "*.cpp")  

FILE(GLOB EXT_LIST "WormHoles/*.h" "WormHoles/*.cpp")

SOURCE_GROUP(WormHoles FILES ${EXT_LIST})

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_11)

add_executable(${PROJECT_NAME} ${SRC_LIST} ${EXT_LIST})




