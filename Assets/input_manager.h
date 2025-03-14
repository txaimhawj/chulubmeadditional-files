#pragma once

#include <unordered_map>
#include <functional>
#include <vector>
#include <string>
#include <memory>
#include <glm/glm.hpp>

#include "../core/ecs.h"

namespace CHULUBME {

// Key codes
enum class KeyCode {
    Unknown = 0,
    // Keyboard
    A = 65, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
    Num0 = 48, Num1, Num2, Num3, Num4, Num5, Num6, Num7, Num8, Num9,
    Escape = 256, Enter, Tab, Backspace, Insert, Delete, Right, Left, Down, Up,
    PageUp, PageDown, Home, End, CapsLock, ScrollLock, NumLock, PrintScreen, Pause,
    F1 = 290, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,
    LeftShift = 340, LeftControl, LeftAlt, LeftSuper,
    RightShift, RightControl, RightAlt, RightSuper,
    Space = 32, Apostrophe = 39, Comma = 44, Minus, Period, Slash,
    Semicolon = 59, Equal = 61, LeftBracket = 91, Backslash, RightBracket,
    GraveAccent = 96, 
    // Keypad
    KP0 = 320, KP1, KP2, KP3, KP4, KP5, KP6, KP7, KP8, KP9,
    KPDecimal, KPDivide, KPMultiply, KPSubtract, KPAdd, KPEnter, KPEqual,
    // Mouse
    MouseLeft = 400, MouseRight, MouseMiddle, MouseX1, MouseX2
};

// Mouse button codes
enum class MouseButton {
    Left = 0,
    Right = 1,
    Middle = 2,
    X1 = 3,
    X2 = 4
};

// Input action types
enum class InputAction {
    Press = 0,
    Release = 1,
    Repeat = 2
};

// Input modifier flags
enum class InputMod {
    None = 0,
    Shift = 1,
    Control = 2,
    Alt = 4,
    Super = 8,
    CapsLock = 16,
    NumLock = 32
};

/**
 * @brief Input manager class for handling keyboard and mouse input
 */
class InputManager {
public:
    // Singleton instance
    static InputManager& Instance();

    // Initialize the input manager
    bool Initialize();

    // Shutdown the input manager
    void Shutdown();

    // Update the input state
    void Update();

    // Process a key event
    void ProcessKeyEvent(KeyCode key, InputAction action, InputMod mods);

    // Process a mouse button event
    void ProcessMouseButtonEvent(MouseButton button, InputAction action, InputMod mods);

    // Process a mouse move event
    void ProcessMouseMoveEvent(double xpos, double ypos);

    // Process a mouse scroll event
    void ProcessMouseScrollEvent(double xoffset, double yoffset);

    // Check if a key is currently pressed
    bool IsKeyPressed(KeyCode key) const;

    // Check if a key was just pressed this frame
    bool IsKeyJustPressed(KeyCode key) const;

    // Check if a key was just released this frame
    bool IsKeyJustReleased(KeyCode key) const;

    // Check if a mouse button is currently pressed
    bool IsMouseButtonPressed(MouseButton button) const;

    // Check if a mouse button was just pressed this frame
    bool IsMouseButtonJustPressed(MouseButton button) const;

    // Check if a mouse button was just released this frame
    bool IsMouseButtonJustReleased(MouseButton button) const;

    // Get the current mouse position
    glm::vec2 GetMousePosition() const;

    // Get the mouse position delta since last frame
    glm::vec2 GetMouseDelta() const;

    // Get the mouse scroll delta since last frame
    glm::vec2 GetMouseScrollDelta() const;

    // Set the mouse position
    void SetMousePosition(double xpos, double ypos);

    // Set whether the cursor is visible
    void SetCursorVisible(bool visible);

    // Register a key callback
    void RegisterKeyCallback(std::function<void(KeyCode, InputAction, InputMod)> callback);

    // Register a mouse button callback
    void RegisterMouseButtonCallback(std::function<void(MouseButton, InputAction, InputMod)> callback);

    // Register a mouse move callback
    void RegisterMouseMoveCallback(std::function<void(double, double)> callback);

    // Register a mouse scroll callback
    void RegisterMouseScrollCallback(std::function<void(double, double)> callback);

private:
    // Private constructor for singleton
    InputManager();
    ~InputManager();

    // Deleted copy constructor and assignment operator
    InputManager(const InputManager&) = delete;
    InputManager& operator=(const InputManager&) = delete;

    // Key state maps
    std::unordered_map<KeyCode, bool> m_keyStates;
    std::unordered_map<KeyCode, bool> m_keyJustPressed;
    std::unordered_map<KeyCode, bool> m_keyJustReleased;

    // Mouse button state maps
    std::unordered_map<MouseButton, bool> m_mouseButtonStates;
    std::unordered_map<MouseButton, bool> m_mouseButtonJustPressed;
    std::unordered_map<MouseButton, bool> m_mouseButtonJustReleased;

    // Mouse position
    glm::vec2 m_mousePosition;
    glm::vec2 m_lastMousePosition;
    glm::vec2 m_mouseDelta;

    // Mouse scroll
    glm::vec2 m_mouseScrollDelta;

    // Callbacks
    std::vector<std::function<void(KeyCode, InputAction, InputMod)>> m_keyCallbacks;
    std::vector<std::function<void(MouseButton, InputAction, InputMod)>> m_mouseButtonCallbacks;
    std::vector<std::function<void(double, double)>> m_mouseMoveCallbacks;
    std::vector<std::function<void(double, double)>> m_mouseScrollCallbacks;
};

/**
 * @brief Input component for entity input handling
 */
class InputComponent : public Component {
public:
    InputComponent();
    ~InputComponent() = default;

    // Initialize the input component
    void Initialize() override;

    // Finalize the input component
    void Finalize() override;

    // Register a key callback for this entity
    void RegisterKeyCallback(std::function<void(KeyCode, InputAction, InputMod)> callback);

    // Register a mouse button callback for this entity
    void RegisterMouseButtonCallback(std::function<void(MouseButton, InputAction, InputMod)> callback);

    // Register a mouse move callback for this entity
    void RegisterMouseMoveCallback(std::function<void(double, double)> callback);

    // Register a mouse scroll callback for this entity
    void RegisterMouseScrollCallback(std::function<void(double, double)> callback);

private:
    // Callback handles for cleanup
    int m_keyCallbackHandle;
    int m_mouseButtonCallbackHandle;
    int m_mouseMoveCallbackHandle;
    int m_mouseScrollCallbackHandle;
};

/**
 * @brief Input system for processing input events
 */
class InputSystem : public System {
public:
    InputSystem(EntityManager* manager);
    ~InputSystem() = default;

    // Initialize the input system
    void Initialize() override;

    // Update the input system
    void Update(float deltaTime) override;

    // Called when an entity is added to this system
    void OnEntityAdded(Entity entity) override;

    // Called when an entity is removed from this system
    void OnEntityRemoved(Entity entity) override;

private:
    // Input manager reference
    InputManager& m_inputManager;
};

// Implementation

inline InputManager& InputManager::Instance() {
    static InputManager instance;
    return instance;
}

inline InputManager::InputManager()
    : m_mousePosition(0.0f, 0.0f)
    , m_lastMousePosition(0.0f, 0.0f)
    , m_mouseDelta(0.0f, 0.0f)
    , m_mouseScrollDelta(0.0f, 0.0f)
{
}

inline InputManager::~InputManager() {
    Shutdown();
}

inline bool InputManager::Initialize() {
    // Clear all state maps
    m_keyStates.clear();
    m_keyJustPressed.clear();
    m_keyJustReleased.clear();
    m_mouseButtonStates.clear();
    m_mouseButtonJustPressed.clear();
    m_mouseButtonJustReleased.clear();

    // Reset mouse position and delta
    m_mousePosition = glm::vec2(0.0f, 0.0f);
    m_lastMousePosition = glm::vec2(0.0f, 0.0f);
    m_mouseDelta = glm::vec2(0.0f, 0.0f);
    m_mouseScrollDelta = glm::vec2(0.0f, 0.0f);

    return true;
}

inline void InputManager::Shutdown() {
    // Clear all callbacks
    m_keyCallbacks.clear();
    m_mouseButtonCallbacks.clear();
    m_mouseMoveCallbacks.clear();
    m_mouseScrollCallbacks.clear();
}

inline void InputManager::Update() {
    // Update mouse delta
    m_mouseDelta = m_mousePosition - m_lastMousePosition;
    m_lastMousePosition = m_mousePosition;

    // Clear just pressed and just released flags
    m_keyJustPressed.clear();
    m_keyJustReleased.clear();
    m_mouseButtonJustPressed.clear();
    m_mouseButtonJustReleased.clear();

    // Reset scroll delta
    m_mouseScrollDelta = glm::vec2(0.0f, 0.0f);
}

inline void InputManager::ProcessKeyEvent(KeyCode key, InputAction action, InputMod mods) {
    // Update key state
    if (action == InputAction::Press) {
        m_keyStates[key] = true;
        m_keyJustPressed[key] = true;
    } else if (action == InputAction::Release) {
        m_keyStates[key] = false;
        m_keyJustReleased[key] = true;
    }

    // Call key callbacks
    for (const auto& callback : m_keyCallbacks) {
        callback(key, action, mods);
    }
}

inline void InputManager::ProcessMouseButtonEvent(MouseButton button, InputAction action, InputMod mods) {
    // Update mouse button state
    if (action == InputAction::Press) {
        m_mouseButtonStates[button] = true;
        m_mouseButtonJustPressed[button] = true;
    } else if (action == InputAction::Release) {
        m_mouseButtonStates[button] = false;
        m_mouseButtonJustReleased[button] = true;
    }

    // Call mouse button callbacks
    for (const auto& callback : m_mouseButtonCallbacks) {
        callback(button, action, mods);
    }
}

inline void InputManager::ProcessMouseMoveEvent(double xpos, double ypos) {
    // Update mouse position
    m_mousePosition = glm::vec2(static_cast<float>(xpos), static_cast<float>(ypos));

    // Call mouse move callbacks
    for (const auto& callback : m_mouseMoveCallbacks) {
        callback(xpos, ypos);
    }
}

inline void InputManager::ProcessMouseScrollEvent(double xoffset, double yoffset) {
    // Update mouse scroll delta
    m_mouseScrollDelta = glm::vec2(static_cast<float>(xoffset), static_cast<float>(yoffset));

    // Call mouse scroll callbacks
    for (const auto& callback : m_mouseScrollCallbacks) {
        callback(xoffset, yoffset);
    }
}

inline bool InputManager::IsKeyPressed(KeyCode key) const {
    auto it = m_keyStates.find(key);
    return (it != m_keyStates.end()) ? it->second : false;
}

inline bool InputManager::IsKeyJustPressed(KeyCode key) const {
    auto it = m_keyJustPressed.find(key);
    return (it != m_keyJustPressed.end()) ? it->second : false;
}

inline bool InputManager::IsKeyJustReleased(KeyCode key) const {
    auto it = m_keyJustReleased.find(key);
    return (it != m_keyJustReleased.end()) ? it->second : false;
}

inline bool InputManager::IsMouseButtonPressed(MouseButton button) const {
    auto it = m_mouseButtonStates.find(button);
    return (it != m_mouseButtonStates.end()) ? it->second : false;
}

inline bool InputManager::IsMouseButtonJustPressed(MouseButton button) const {
    auto it = m_mouseButtonJustPressed.find(button);
    return (it != m_mouseButtonJustPressed.end()) ? it->second : false;
}

inline bool InputManager::IsMouseButtonJustReleased(MouseButton button) const {
    auto it = m_mouseButtonJustReleased.find(button);
    return (it != m_mouseButtonJustReleased.end()) ? it->second : false;
}

inline glm::vec2 InputManager::GetMousePosition() const {
    return m_mousePosition;
}

inline glm::vec2 InputManager::GetMouseDelta() const {
    return m_mouseDelta;
}

inline glm::vec2 InputManager::GetMouseScrollDelta() const {
    return m_mouseScrollDelta;
}

inline void InputManager::SetMousePosition(double xpos, double ypos) {
    m_mousePosition = glm::vec2(static_cast<float>(xpos), static_cast<float>(ypos));
    m_lastMousePosition = m_mousePosition;
}

inline void InputManager::RegisterKeyCallback(std::function<void(KeyCode, InputAction, InputMod)> callback) {
    m_keyCallbacks.push_back(callback);
}

inline void InputManager::RegisterMouseButtonCallback(std::function<void(MouseButton, InputAction, InputMod)> callback) {
    m_mouseButtonCallbacks.push_back(callback);
}

inline void InputManager::RegisterMouseMoveCallback(std::function<void(double, double)> callback) {
    m_mouseMoveCallbacks.push_back(callback);
}

inline void InputManager::RegisterMouseScrollCallback(std::function<void(double, double)> callback) {
    m_mouseScrollCallbacks.push_back(callback);
}

} // namespace CHULUBME
