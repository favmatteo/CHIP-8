# Chip-8 Emulator
### My first emulator

This project allows you to emulate all games written for [CHIP-8](https://en.wikipedia.org/wiki/CHIP-8)

## Features
1. [x] Video
2. [x] Audio
3. [x] Multi-platform
4. [ ] Visualisation of registers (for debug)
5. [ ] Installation script

## Installation
This emulator requires [SDL](https://www.libsdl.org/) and [SDL-MIXER](https://www.libsdl.org/projects/SDL_mixer/) library

Install the dependencies and compile with

```sh
cmake .
cmake --build .
./CHIP_8
```
_In the case of an error, remove the problematic compilation flags from the CMakeLists file_