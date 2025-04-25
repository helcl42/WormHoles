cmake_minimum_required(VERSION 3.10)
project(UnitTests)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
SET(SOURCE_GROUP_DELIMITER "/")

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

add_executable(UnitTests ${TEST_SOURCES})
target_link_libraries(UnitTests gtest gtest_main)

add_test(NAME UnitTests COMMAND UnitTests)