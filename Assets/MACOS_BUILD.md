# CHULUBME Game Engine - macOS Build Guide for Apple Silicon (M4)

This guide provides detailed instructions for building and running the CHULUBME game engine on macOS with Apple Silicon (M4) chips.

## Prerequisites

### Install Xcode Command Line Tools

First, ensure you have Xcode Command Line Tools installed:

```bash
xcode-select --install
```

### Install Homebrew

If you don't have Homebrew installed, install it with:

```bash
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
```

After installation, make sure Homebrew is in your PATH:

```bash
# For Apple Silicon Macs
echo 'eval "$(/opt/homebrew/bin/brew shellenv)"' >> ~/.zprofile
eval "$(/opt/homebrew/bin/brew shellenv)"
```

### Install Dependencies

Install the required dependencies using Homebrew:

```bash
# Core dependencies
brew install cmake
brew install sdl2
brew install glew
brew install jsoncpp
brew install nlohmann-json

# Optional but recommended
brew install ccache  # For faster rebuilds
brew install ninja   # For faster builds
```

## Setting Up the Project

### Clone the Repository

If you haven't already, clone the repository:

```bash
git clone https://github.com/your-username/chulubme-moba.git
cd chulubme-moba
```

### Initialize Submodules

Initialize and update the submodules (for ImGui and other dependencies):

```bash
git submodule update --init --recursive
```

## Building with CMake

### Create a Build Directory

```bash
mkdir -p build
cd build
```

### Configure with CMake

For Apple Silicon, you need to specify the architecture:

```bash
cmake .. -DCMAKE_OSX_ARCHITECTURES=arm64 -G Ninja
```

If you don't have Ninja installed, you can omit the `-G Ninja` flag.

### Build the Project

```bash
cmake --build . --config Release
```

### Run the Hero Test Environment

```bash
./test_hero_environment
```

## Manual Compilation (Alternative)

If you prefer to compile manually or are experiencing issues with CMake, you can use the following command:

```bash
clang++ -std=c++17 code/engine/test_hero_environment.cpp -o test_hero_environment \
    -I. -Iexternal/imgui -Iexternal/imgui/backends \
    -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo \
    -lSDL2 -lGLEW -ljsoncpp \
    -arch arm64
```

## Creating a CMakeLists.txt for macOS

If you need to create or modify a CMakeLists.txt file for the project, here's a template that works well for macOS with Apple Silicon:

```cmake
cmake_minimum_required(VERSION 3.14)
project(CHULUBME VERSION 0.1.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set architecture for Apple Silicon
if(APPLE)
    set(CMAKE_OSX_ARCHITECTURES "arm64")
endif()

# Find packages
find_package(SDL2 REQUIRED)
find_package(GLEW REQUIRED)
find_package(jsoncpp REQUIRED)
find_package(OpenGL REQUIRED)

# Include directories
include_directories(
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/external/imgui
    ${CMAKE_SOURCE_DIR}/external/imgui/backends
    ${SDL2_INCLUDE_DIRS}
    ${GLEW_INCLUDE_DIRS}
    ${JSONCPP_INCLUDE_DIRS}
)

# Add ImGui source files
set(IMGUI_SOURCES
    external/imgui/imgui.cpp
    external/imgui/imgui_demo.cpp
    external/imgui/imgui_draw.cpp
    external/imgui/imgui_tables.cpp
    external/imgui/imgui_widgets.cpp
    external/imgui/backends/imgui_impl_sdl2.cpp
    external/imgui/backends/imgui_impl_opengl3.cpp
)

# Add source files
file(GLOB_RECURSE ENGINE_SOURCES
    code/engine/core/*.cpp
    code/engine/rendering/*.cpp
    code/engine/physics/*.cpp
    code/engine/input/*.cpp
    code/engine/audio/*.cpp
    code/engine/resource/*.cpp
    code/engine/network/*.cpp
    code/engine/blockchain_interface/*.cpp
    code/engine/gameplay/*.cpp
)

# Add executable
add_executable(test_hero_environment
    code/engine/test_hero_environment.cpp
    ${ENGINE_SOURCES}
    ${IMGUI_SOURCES}
)

# Find macOS specific frameworks
if(APPLE)
    find_library(COCOA_LIBRARY Cocoa)
    find_library(IOKIT_LIBRARY IOKit)
    find_library(COREVIDEO_LIBRARY CoreVideo)
    
    target_link_libraries(test_hero_environment
        ${COCOA_LIBRARY}
        ${IOKIT_LIBRARY}
        ${COREVIDEO_LIBRARY}
    )
endif()

# Link libraries
target_link_libraries(test_hero_environment
    ${SDL2_LIBRARIES}
    ${GLEW_LIBRARIES}
    ${JSONCPP_LIBRARIES}
    ${OPENGL_LIBRARIES}
)
```

## Troubleshooting macOS-Specific Issues

### OpenGL Deprecation Warnings

macOS has deprecated OpenGL, but it still works. You might see deprecation warnings during compilation. You can suppress these warnings by adding:

```cmake
if(APPLE)
    add_compile_options(-Wno-deprecated-declarations)
endif()
```

### Metal as an Alternative

For future development, consider using Metal instead of OpenGL for better performance on Apple Silicon. This would require implementing a Metal backend for the rendering system.

### Arm64 vs x86_64 Issues

If you're experiencing architecture-related issues:

1. Make sure all dependencies are installed for arm64:
   ```bash
   arch -arm64 brew install <package>
   ```

2. Check the architecture of your binaries:
   ```bash
   lipo -info <binary>
   ```

3. Force compilation for arm64:
   ```bash
   cmake .. -DCMAKE_OSX_ARCHITECTURES=arm64
   ```

### SDL2 Issues

If you encounter issues with SDL2:

1. Try installing SDL2 from source:
   ```bash
   brew install --build-from-source sdl2
   ```

2. Verify SDL2 is properly linked:
   ```bash
   otool -L test_hero_environment | grep SDL2
   ```

## Performance Optimization for M4

The M4 chip offers excellent performance, but to get the most out of it:

1. **Use Metal**: Consider implementing a Metal backend for rendering
2. **Enable multithreading**: The M4 has multiple cores, so use them
3. **Optimize memory access patterns**: The M4's unified memory architecture benefits from coherent memory access
4. **Use Apple's performance tools**: Instruments can help identify bottlenecks

## Next Steps

After successfully building the CHULUBME game engine on your M4 MacBook Pro:

1. Load the sample heroes from `sample_heroes.json`
2. Create and edit your own heroes using the Hero Editor
3. Test hero abilities in the Hero Test window
4. Integrate with the blockchain components

## Additional Resources

- [Apple Developer Documentation](https://developer.apple.com/documentation/)
- [SDL2 on macOS](https://wiki.libsdl.org/SDL2/FAQMacOS)
- [OpenGL on macOS](https://developer.apple.com/opengl/)
- [Metal Programming Guide](https://developer.apple.com/metal/)

If you encounter any issues specific to your M4 MacBook Pro, please check the project's issue tracker or contact the development team for assistance.
