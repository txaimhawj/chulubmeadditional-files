#include <iostream>
#include <memory>
#include <string>

#include "core/ecs.h"
#include "core/engine.h"
#include "core/memory.h"
#include "rendering/renderer.h"
#include "input/input_manager.h"
#include "blockchain_interface/blockchain_interface.h"

using namespace CHULUBME;

// Test components
class TestComponent : public Component {
public:
    TestComponent(const std::string& name = "TestComponent") : m_name(name) {
        std::cout << "TestComponent created: " << m_name << std::endl;
    }
    
    ~TestComponent() {
        std::cout << "TestComponent destroyed: " << m_name << std::endl;
    }
    
    void Initialize() override {
        std::cout << "TestComponent initialized: " << m_name << std::endl;
    }
    
    void Finalize() override {
        std::cout << "TestComponent finalized: " << m_name << std::endl;
    }
    
    std::string GetName() const { return m_name; }
    
private:
    std::string m_name;
};

// Test system
class TestSystem : public System {
public:
    TestSystem(EntityManager* manager) : System(manager) {
        std::cout << "TestSystem created" << std::endl;
        RequireComponent<TestComponent>();
    }
    
    ~TestSystem() {
        std::cout << "TestSystem destroyed" << std::endl;
    }
    
    void Initialize() override {
        std::cout << "TestSystem initialized" << std::endl;
    }
    
    void Update(float deltaTime) override {
        for (EntityID entityID : GetEntities()) {
            Entity entity(entityID, m_manager);
            TestComponent* testComponent = entity.GetComponent<TestComponent>();
            std::cout << "TestSystem updating entity with component: " << testComponent->GetName() << std::endl;
        }
    }
    
    void OnEntityAdded(Entity entity) override {
        std::cout << "Entity added to TestSystem: " << entity.GetID() << std::endl;
    }
    
    void OnEntityRemoved(Entity entity) override {
        std::cout << "Entity removed from TestSystem: " << entity.GetID() << std::endl;
    }
};

// Test input handling
class TestInputHandler {
public:
    TestInputHandler() {
        // Register key callback
        InputManager::Instance().RegisterKeyCallback(
            [this](KeyCode key, InputAction action, InputMod mods) {
                if (action == InputAction::Press) {
                    std::cout << "Key pressed: " << static_cast<int>(key) << std::endl;
                }
            }
        );
        
        // Register mouse button callback
        InputManager::Instance().RegisterMouseButtonCallback(
            [this](MouseButton button, InputAction action, InputMod mods) {
                if (action == InputAction::Press) {
                    std::cout << "Mouse button pressed: " << static_cast<int>(button) << std::endl;
                }
            }
        );
        
        // Register mouse move callback
        InputManager::Instance().RegisterMouseMoveCallback(
            [this](double xpos, double ypos) {
                // Only log occasionally to avoid spam
                static int counter = 0;
                if (counter++ % 100 == 0) {
                    std::cout << "Mouse moved: " << xpos << ", " << ypos << std::endl;
                }
            }
        );
    }
};

// Test memory management
void TestMemoryManagement() {
    std::cout << "Testing memory management..." << std::endl;
    
    // Get memory manager
    MemoryManager& memoryManager = MemoryManager::Instance();
    
    // Get heap allocator
    Allocator* heapAllocator = memoryManager.GetAllocator(MemoryManager::AllocatorType::Heap);
    
    // Allocate memory
    void* memory = heapAllocator->Allocate(1024);
    std::cout << "Allocated 1024 bytes from heap allocator" << std::endl;
    
    // Free memory
    heapAllocator->Free(memory);
    std::cout << "Freed memory from heap allocator" << std::endl;
    
    // Create pool allocator
    PoolAllocator* poolAllocator = memoryManager.CreatePoolAllocator("TestPool", 64, 10);
    
    // Allocate from pool
    void* poolMemory1 = poolAllocator->Allocate(64);
    void* poolMemory2 = poolAllocator->Allocate(64);
    std::cout << "Allocated 2 blocks from pool allocator" << std::endl;
    
    // Free from pool
    poolAllocator->Free(poolMemory1);
    poolAllocator->Free(poolMemory2);
    std::cout << "Freed memory from pool allocator" << std::endl;
    
    // Destroy pool allocator
    memoryManager.DestroyAllocator("TestPool");
    std::cout << "Destroyed pool allocator" << std::endl;
    
    // Get memory stats
    MemoryManager::MemoryStats stats = memoryManager.GetMemoryStats();
    std::cout << "Memory stats:" << std::endl;
    std::cout << "  Total allocated: " << stats.totalAllocated << " bytes" << std::endl;
    std::cout << "  Total allocations: " << stats.totalAllocations << std::endl;
}

// Test blockchain interface
void TestBlockchainInterface() {
    std::cout << "Testing blockchain interface..." << std::endl;
    
    // Get blockchain interface
    BlockchainInterface& blockchainInterface = BlockchainInterface::Instance();
    
    // Initialize blockchain interface
    if (!blockchainInterface.Initialize()) {
        std::cout << "Failed to initialize blockchain interface" << std::endl;
        return;
    }
    
    std::cout << "Blockchain interface initialized" << std::endl;
    
    // Create wallet
    if (!blockchainInterface.CreateWallet("password123")) {
        std::cout << "Failed to create wallet" << std::endl;
        return;
    }
    
    std::cout << "Wallet created" << std::endl;
    
    // Get balance
    float balance = blockchainInterface.GetBalance();
    std::cout << "Wallet balance: " << balance << " ILYZ" << std::endl;
    
    // Get owned NFTs
    std::vector<NFT> ownedNFTs = blockchainInterface.GetOwnedNFTs();
    std::cout << "Owned NFTs: " << ownedNFTs.size() << std::endl;
    
    // Shutdown blockchain interface
    blockchainInterface.Shutdown();
    std::cout << "Blockchain interface shut down" << std::endl;
}

// Main function
int main() {
    std::cout << "CHULUBME Game Engine Test" << std::endl;
    
    // Initialize engine
    Engine& engine = Engine::Instance();
    if (!engine.Initialize()) {
        std::cerr << "Failed to initialize engine" << std::endl;
        return 1;
    }
    
    std::cout << "Engine initialized" << std::endl;
    
    // Initialize input manager
    InputManager& inputManager = InputManager::Instance();
    if (!inputManager.Initialize()) {
        std::cerr << "Failed to initialize input manager" << std::endl;
        return 1;
    }
    
    std::cout << "Input manager initialized" << std::endl;
    
    // Initialize memory manager
    MemoryManager& memoryManager = MemoryManager::Instance();
    if (!memoryManager.Initialize()) {
        std::cerr << "Failed to initialize memory manager" << std::endl;
        return 1;
    }
    
    std::cout << "Memory manager initialized" << std::endl;
    
    // Get entity manager
    EntityManager* entityManager = engine.GetEntityManager();
    
    // Register test system
    TestSystem* testSystem = entityManager->RegisterSystem<TestSystem>();
    
    // Create test entities
    Entity entity1 = entityManager->CreateEntity();
    entity1.AddComponent<TestComponent>("Entity1Component");
    
    Entity entity2 = entityManager->CreateEntity();
    entity2.AddComponent<TestComponent>("Entity2Component");
    
    std::cout << "Created test entities" << std::endl;
    
    // Create test input handler
    TestInputHandler inputHandler;
    
    // Test memory management
    TestMemoryManagement();
    
    // Test blockchain interface
    TestBlockchainInterface();
    
    // Set up update callback
    engine.RegisterUpdateCallback([&](float deltaTime) {
        // Update input manager
        inputManager.Update();
        
        // Simulate input
        static float timer = 0.0f;
        timer += deltaTime;
        
        if (timer > 1.0f) {
            timer = 0.0f;
            
            // Simulate key press
            inputManager.ProcessKeyEvent(KeyCode::Space, InputAction::Press, InputMod::None);
            inputManager.ProcessKeyEvent(KeyCode::Space, InputAction::Release, InputMod::None);
            
            // Simulate mouse movement
            static double mouseX = 0.0;
            static double mouseY = 0.0;
            mouseX += 10.0;
            mouseY += 5.0;
            inputManager.ProcessMouseMoveEvent(mouseX, mouseY);
        }
    });
    
    // Set up fixed update callback
    engine.RegisterFixedUpdateCallback([&](float fixedDeltaTime) {
        // Nothing to do in fixed update for this test
    });
    
    // Set up render callback
    engine.RegisterRenderCallback([&]() {
        // Nothing to render for this test
    });
    
    // Run the engine for a short time
    std::cout << "Running engine for 5 seconds..." << std::endl;
    
    // Start a separate thread to stop the engine after 5 seconds
    std::thread stopThread([&]() {
        std::this_thread::sleep_for(std::chrono::seconds(5));
        engine.Stop();
    });
    
    // Run the engine
    engine.Run();
    
    // Wait for stop thread to finish
    stopThread.join();
    
    std::cout << "Engine stopped" << std::endl;
    
    // Destroy test entities
    entity1.Destroy();
    entity2.Destroy();
    
    // Process destructions
    entityManager->ProcessDestructions();
    
    std::cout << "Destroyed test entities" << std::endl;
    
    // Shutdown memory manager
    memoryManager.Shutdown();
    
    std::cout << "Memory manager shut down" << std::endl;
    
    // Shutdown input manager
    inputManager.Shutdown();
    
    std::cout << "Input manager shut down" << std::endl;
    
    // Shutdown engine
    engine.Shutdown();
    
    std::cout << "Engine shut down" << std::endl;
    
    return 0;
}
