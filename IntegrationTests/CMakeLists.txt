cmake_minimum_required(VERSION 3.10)
project(IntegrationTests)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(SOURCE_GROUP_DELIMITER "/")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(DCMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        v1.14.0 # Or a specific release tag/commit hash
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

enable_testing()

include_directories("../WormHoles/")

set(TEST_SOURCES Main.cpp)

add_executable(IntegrationTests ${TEST_SOURCES})
target_link_libraries(IntegrationTests gtest gtest_main)

add_test(NAME IntegrationTests COMMAND IntegrationTests)