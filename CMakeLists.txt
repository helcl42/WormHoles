cmake_minimum_required(VERSION 3.10)
project(WormHoles)

add_subdirectory(WormHoles)
add_subdirectory(UnitTests)
add_subdirectory(IntegrationTests)
add_subdirectory(Example1)
add_subdirectory(Example2)

# testing
enable_testing()