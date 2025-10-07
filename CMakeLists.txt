cmake_minimum_required(VERSION 3.15)

add_subdirectory("lib/entt")
add_subdirectory("lib/raylib" EXCLUDE_FROM_ALL)

project(Asteroids)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/target)

file(GLOB_RECURSE MY_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

add_executable(Asteroids "${MY_SOURCES}")

target_include_directories(Asteroids PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_include_directories(Asteroids PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/lib/raylib/src")
target_include_directories(Asteroids PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/lib/entt/single_include/entt")

target_link_libraries(Asteroids PRIVATE EnTT::EnTT raylib)