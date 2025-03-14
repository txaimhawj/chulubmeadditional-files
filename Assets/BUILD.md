# CHULUBME Game Engine - Build Instructions

This document provides instructions for building the CHULUBME game engine and hero test environment.

## Prerequisites

Before building, ensure you have the following dependencies installed:

- C++17 compatible compiler (GCC 8+, Clang 7+, or MSVC 2019+)
- CMake 3.14+
- SDL2 development libraries
- OpenGL development libraries
- GLEW development libraries
- ImGui library
- jsoncpp library

### Installing Dependencies on Ubuntu

```bash
sudo apt update
sudo apt install build-essential cmake libsdl2-dev libglew-dev libjsoncpp-dev
```

### Installing ImGui

ImGui is included as a submodule in the project. To initialize it:

```bash
git submodule update --init --recursive
```

## Building the Engine and Test Environment

### 1. Create a build directory

```bash
mkdir -p build
cd build
```

### 2. Configure with CMake

```bash
cmake ..
```

### 3. Build the project

```bash
cmake --build .
```

### 4. Run the hero test environment

```bash
./test_hero_environment
```

## Manual Compilation (Alternative)

If you prefer to compile manually:

```bash
g++ -std=c++17 code/engine/test_hero_environment.cpp -o test_hero_environment \
    -I. -Iexternal/imgui -Iexternal/imgui/backends \
    -lSDL2 -lGL -lGLEW -ljsoncpp
```

## Testing the Hero System

1. Run the test environment
2. Load the sample heroes from `sample_heroes.json`
3. Create and edit heroes using the Hero Editor
4. Test hero abilities in the Hero Test window

## Troubleshooting

### Common Build Issues

1. **Missing dependencies**:
   - Ensure all required libraries are installed
   - Check that library paths are correctly set

2. **Compilation errors**:
   - Verify you're using a C++17 compatible compiler
   - Check for syntax errors in the source code

3. **Linker errors**:
   - Ensure all required libraries are linked
   - Check for missing symbols or undefined references

### Runtime Issues

1. **Application crashes on startup**:
   - Check that SDL2 and OpenGL are properly initialized
   - Verify that all required DLLs/shared libraries are in the path

2. **UI rendering issues**:
   - Ensure ImGui is properly initialized
   - Check for OpenGL context errors

3. **Hero loading failures**:
   - Verify the JSON file format is correct
   - Check file permissions for the heroes.json file

## Next Steps

After successfully building and testing the hero system, you can:

1. Integrate with the blockchain components
2. Implement the full game mechanics
3. Create additional heroes and abilities
4. Develop the game client and server

For more information, refer to the hero creation workflow documentation.
