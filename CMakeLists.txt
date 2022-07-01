cmake_minimum_required(VERSION 3.22)
project(CHIP_8)

set(CMAKE_CXX_STANDARD 20)

add_executable(CHIP_8 main.cpp Chip8.hpp Chip8.cpp)

# FMT Library
target_link_libraries(CHIP_8 PUBLIC fmt)

# SFML Library
target_link_libraries(CHIP_8 PUBLIC sfml-graphics sfml-window sfml-system)

# Compiler Flag
if (MSVC)
    target_compile_options(CHIP_8 PRIVATE /W4 /WX)
else ()
#   target_compile_options(CHIP_8 PRIVATE -fmodules-ts -lsfml-graphics -lsfml-window -lsfml-system -Wall -Wextra -Wpedantic)
    target_compile_options(CHIP_8 PRIVATE -Wall -Wextra -Wpedantic)
endif ()
