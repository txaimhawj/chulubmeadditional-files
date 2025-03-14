# CHULUBME Game Engine Implementation

## Overview
This document provides a compilation guide for the CHULUBME game engine with blockchain integration.

## Directory Structure
```
moba_game_project/
├── code/
│   ├── blockchain/
│   │   └── custom/
│   │       ├── core/
│   │       │   ├── blockchain.go
│   │       │   ├── token_economics.go
│   │       │   └── nft_system.go
│   │       ├── consensus/
│   │       │   └── proof_of_play.go
│   │       ├── crypto/
│   │       │   └── crypto.go
│   │       ├── network/
│   │       │   └── network.go
│   │       └── wallet/
│   │           └── wallet.go
│   └── engine/
│       ├── core/
│       │   ├── ecs.h
│       │   ├── engine.h
│       │   └── memory.h
│       ├── rendering/
│       │   └── renderer.h
│       ├── physics/
│       ├── input/
│       │   └── input_manager.h
│       ├── audio/
│       ├── resource/
│       ├── network/
│       ├── blockchain_interface/
│       │   └── blockchain_interface.h
│       └── test_engine.cpp
└── docs/
    ├── blockchain_analysis.md
    ├── game_engine_analysis.md
    ├── blockchain_game_integration.md
    ├── implementation_recommendations.md
    ├── implementation_roadmap.md
    └── summary_and_next_steps.md
```

## Compilation Instructions

### Prerequisites
- C++17 compatible compiler (GCC 8+, Clang 7+, or MSVC 2019+)
- CMake 3.14+
- Go 1.16+
- OpenGL development libraries
- GLM math library

### Building the Blockchain Components
1. Navigate to the blockchain directory:
   ```
   cd moba_game_project/code/blockchain/custom
   ```

2. Build the Go blockchain components:
   ```
   go build -o libchulubmechain.so -buildmode=c-shared
   ```

### Building the Game Engine
1. Create a build directory:
   ```
   mkdir -p moba_game_project/build
   cd moba_game_project/build
   ```

2. Run CMake:
   ```
   cmake ..
   ```

3. Build the engine:
   ```
   cmake --build .
   ```

4. Run the test application:
   ```
   ./test_engine
   ```

## Key Components

### Core ECS Architecture
The Entity Component System (ECS) architecture provides the foundation for the game engine:
- `Entity`: A unique identifier with associated components
- `Component`: Data containers attached to entities
- `System`: Logic that processes entities with specific component combinations
- `EntityManager`: Manages entities, components, and systems

### Game Loop and Timing
The engine implements a fixed timestep game loop with:
- Frame rate limiting
- Fixed update for physics and gameplay logic
- Variable update for rendering
- Time tracking for delta time calculation

### Rendering System
The rendering system includes:
- Renderer singleton for managing the rendering pipeline
- Shader, Texture, Mesh, and Material classes for rendering resources
- Camera component for scene viewing
- Transform component for entity positioning
- MeshRenderer component for rendering meshes
- RenderSystem for processing all renderable entities

### Input Handling
The input system provides:
- Keyboard and mouse input tracking
- Input state management (pressed, just pressed, just released)
- Callback registration for input events
- InputComponent for entity-specific input handling
- InputSystem for processing input events

### Memory Management
The memory management system includes:
- Various allocator types (Linear, Pool, Stack, Heap)
- MemoryManager singleton for allocator management
- Custom STL allocator for container integration
- Memory tracking utilities for debugging

### Blockchain Integration
The blockchain interface provides:
- Access to the blockchain, wallet, NFT system, and token economics
- WalletComponent for entity blockchain interaction
- NFTComponent for entity NFT representation
- BlockchainSystem for processing blockchain interactions
- GameRewardSystem for calculating and distributing rewards

## Next Steps
1. Implement the rendering system backend (OpenGL/DirectX)
2. Develop the physics system for collision detection and response
3. Create the network layer for client-server communication
4. Implement the audio system for sound and music
5. Develop the resource management system for asset loading
6. Create game-specific systems (champions, abilities, map)
7. Implement the UI system for menus and HUD
8. Integrate with the blockchain for NFT skins and rewards

## Testing
The test_engine.cpp file provides a basic test harness for verifying the engine components:
- ECS architecture testing with test components and systems
- Game loop testing with update callbacks
- Input handling testing with simulated input events
- Memory management testing with various allocators
- Blockchain interface testing with wallet creation and balance checking

## Conclusion
The CHULUBME game engine provides a solid foundation for building a competitive MOBA game with blockchain integration. The modular architecture allows for easy extension and customization, while the blockchain integration enables innovative gameplay features like NFT skins and token rewards.
