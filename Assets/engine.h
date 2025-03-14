#pragma once

#include <chrono>
#include <functional>
#include <string>
#include <thread>
#include <atomic>
#include <memory>

#include "ecs.h"

namespace CHULUBME {

/**
 * @brief Game engine class that manages the main game loop and timing
 */
class Engine {
public:
    // Singleton instance
    static Engine& Instance();

    // Initialize the engine
    bool Initialize();

    // Shutdown the engine
    void Shutdown();

    // Run the main game loop
    void Run();

    // Stop the main game loop
    void Stop();

    // Get the entity manager
    EntityManager* GetEntityManager() { return m_entityManager.get(); }

    // Get the current delta time (time between frames)
    float GetDeltaTime() const { return m_deltaTime; }

    // Get the current frame rate
    float GetFrameRate() const { return m_frameRate; }

    // Get the target frame rate
    float GetTargetFrameRate() const { return m_targetFrameRate; }

    // Set the target frame rate (0 for unlimited)
    void SetTargetFrameRate(float targetFrameRate) { m_targetFrameRate = targetFrameRate; }

    // Get the fixed update rate (for physics and gameplay logic)
    float GetFixedUpdateRate() const { return m_fixedUpdateRate; }

    // Set the fixed update rate
    void SetFixedUpdateRate(float fixedUpdateRate) { m_fixedUpdateRate = fixedUpdateRate; }

    // Register update callback
    void RegisterUpdateCallback(std::function<void(float)> callback) { m_updateCallback = callback; }

    // Register render callback
    void RegisterRenderCallback(std::function<void()> callback) { m_renderCallback = callback; }

    // Register fixed update callback
    void RegisterFixedUpdateCallback(std::function<void(float)> callback) { m_fixedUpdateCallback = callback; }

    // Register initialization callback
    void RegisterInitCallback(std::function<bool()> callback) { m_initCallback = callback; }

    // Register shutdown callback
    void RegisterShutdownCallback(std::function<void()> callback) { m_shutdownCallback = callback; }

private:
    // Private constructor for singleton
    Engine();
    ~Engine();

    // Deleted copy constructor and assignment operator
    Engine(const Engine&) = delete;
    Engine& operator=(const Engine&) = delete;

    // Update game state
    void Update(float deltaTime);

    // Render game state
    void Render();

    // Fixed update for physics and gameplay logic
    void FixedUpdate(float fixedDeltaTime);

    // Entity manager
    std::unique_ptr<EntityManager> m_entityManager;

    // Time tracking
    std::chrono::high_resolution_clock::time_point m_lastFrameTime;
    std::chrono::high_resolution_clock::time_point m_lastFixedUpdateTime;
    float m_deltaTime;
    float m_frameRate;
    float m_targetFrameRate;
    float m_fixedUpdateRate;
    float m_fixedUpdateAccumulator;

    // Running state
    std::atomic<bool> m_running;

    // Callbacks
    std::function<void(float)> m_updateCallback;
    std::function<void()> m_renderCallback;
    std::function<void(float)> m_fixedUpdateCallback;
    std::function<bool()> m_initCallback;
    std::function<void()> m_shutdownCallback;
};

// Implementation

inline Engine& Engine::Instance() {
    static Engine instance;
    return instance;
}

inline Engine::Engine()
    : m_entityManager(std::make_unique<EntityManager>())
    , m_deltaTime(0.0f)
    , m_frameRate(0.0f)
    , m_targetFrameRate(60.0f)
    , m_fixedUpdateRate(60.0f)
    , m_fixedUpdateAccumulator(0.0f)
    , m_running(false)
{
}

inline Engine::~Engine() {
    Shutdown();
}

inline bool Engine::Initialize() {
    // Reset time tracking
    m_lastFrameTime = std::chrono::high_resolution_clock::now();
    m_lastFixedUpdateTime = m_lastFrameTime;
    m_deltaTime = 0.0f;
    m_frameRate = 0.0f;
    m_fixedUpdateAccumulator = 0.0f;

    // Call initialization callback if registered
    if (m_initCallback) {
        return m_initCallback();
    }

    return true;
}

inline void Engine::Shutdown() {
    // Stop the game loop if running
    Stop();

    // Call shutdown callback if registered
    if (m_shutdownCallback) {
        m_shutdownCallback();
    }

    // Clear entity manager
    m_entityManager.reset(new EntityManager());
}

inline void Engine::Run() {
    if (m_running) {
        return;
    }

    m_running = true;
    m_lastFrameTime = std::chrono::high_resolution_clock::now();
    m_lastFixedUpdateTime = m_lastFrameTime;

    // Main game loop
    while (m_running) {
        // Calculate delta time
        auto currentTime = std::chrono::high_resolution_clock::now();
        auto frameTimeDuration = std::chrono::duration_cast<std::chrono::microseconds>(currentTime - m_lastFrameTime);
        m_deltaTime = frameTimeDuration.count() / 1000000.0f;
        m_lastFrameTime = currentTime;

        // Calculate frame rate
        m_frameRate = 1.0f / m_deltaTime;

        // Process entity destructions
        m_entityManager->ProcessDestructions();

        // Update game state
        Update(m_deltaTime);

        // Accumulate fixed update time
        m_fixedUpdateAccumulator += m_deltaTime;
        
        // Fixed time step for physics and gameplay logic
        const float fixedDeltaTime = 1.0f / m_fixedUpdateRate;
        while (m_fixedUpdateAccumulator >= fixedDeltaTime) {
            FixedUpdate(fixedDeltaTime);
            m_fixedUpdateAccumulator -= fixedDeltaTime;
        }

        // Render game state
        Render();

        // Frame rate limiting
        if (m_targetFrameRate > 0) {
            const float targetFrameTime = 1.0f / m_targetFrameRate;
            const float elapsedTime = std::chrono::duration_cast<std::chrono::microseconds>(
                std::chrono::high_resolution_clock::now() - currentTime).count() / 1000000.0f;
            
            if (elapsedTime < targetFrameTime) {
                std::this_thread::sleep_for(std::chrono::microseconds(
                    static_cast<int>((targetFrameTime - elapsedTime) * 1000000)));
            }
        }
    }
}

inline void Engine::Stop() {
    m_running = false;
}

inline void Engine::Update(float deltaTime) {
    // Update all systems
    m_entityManager->UpdateSystems(deltaTime);

    // Call update callback if registered
    if (m_updateCallback) {
        m_updateCallback(deltaTime);
    }
}

inline void Engine::Render() {
    // Render all systems
    m_entityManager->RenderSystems();

    // Call render callback if registered
    if (m_renderCallback) {
        m_renderCallback();
    }
}

inline void Engine::FixedUpdate(float fixedDeltaTime) {
    // Call fixed update callback if registered
    if (m_fixedUpdateCallback) {
        m_fixedUpdateCallback(fixedDeltaTime);
    }
}

} // namespace CHULUBME
