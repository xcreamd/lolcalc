cmake_minimum_required(VERSION 3.10)
project(LeagueDamageCalculator)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE SOURCES
    "src/*.cpp"
)

add_executable(lol_calculator ${SOURCES})

target_include_directories(lol_calculator
    PUBLIC include
    PUBLIC include/components
    PUBLIC libs
)

if(MSVC)
  target_compile_options(lol_calculator PRIVATE /W4)
else()
  target_compile_options(lol_calculator PRIVATE -Wall -Wextra -Wpedantic)
endif()

message(STATUS "Project configured. Use 'cmake --build build' to compile.")
