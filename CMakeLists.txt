cmake_minimum_required(VERSION 3.25)

project(
    pong
    VERSION 0.0.1
    LANGUAGES C
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")
if(CMAKE_EXPORT_COMPILE_COMMANDS)
  set(CMAKE_C_STANDARD_INCLUDE_DIRECTORIES 
      ${CMAKE_C_IMPLICIT_INCLUDE_DIRECTORIES})
endif()

set(RAYLIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/raylib")

add_executable(
    pong
    "main.c"
)

target_link_directories(
    pong
    PRIVATE
    "${RAYLIB_DIR}/lib"
)
target_link_libraries(pong PRIVATE raylib)

target_include_directories(
    pong
    PRIVATE
    "${RAYLIB_DIR}/include"
)

set_property(TARGET pong PROPERTY C_STANDARD 11)
