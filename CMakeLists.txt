cmake_minimum_required(VERSION 3.22)
project(CHIP_8)

set(CMAKE_CXX_STANDARD 20)

add_executable(CHIP_8 main.cpp Chip8.hpp Chip8.cpp Platform.cpp Platform.hpp)

# SDL2
find_package(SDL2 REQUIRED SDL2)
target_link_libraries(CHIP_8 PRIVATE SDL2)

# SDL2 MIXER
find_package(SDL2 REQUIRED SDL2_mixer)
target_link_libraries(CHIP_8 PRIVATE SDL2_mixer)

# Compiler Flag
if (MSVC)
    target_compile_options(CHIP_8 PRIVATE /W4 /WX)
else ()
    target_compile_options(CHIP_8 PRIVATE -O3 -Wall -Wextra -Wpedantic -Werror)
endif ()
