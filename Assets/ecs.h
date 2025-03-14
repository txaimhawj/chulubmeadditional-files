#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>
#include <typeindex>
#include <memory>
#include <vector>
#include <bitset>
#include <array>
#include <queue>
#include <set>

namespace CHULUBME {

// Forward declarations
class Entity;
class Component;
class System;
class EntityManager;

// Type definitions
using EntityID = std::uint32_t;
using ComponentID = std::uint32_t;
using SystemID = std::uint32_t;

// Maximum number of components and entities
constexpr std::size_t MAX_COMPONENTS = 64;
constexpr std::size_t MAX_ENTITIES = 10000;

// Component mask to determine which components an entity has
using ComponentMask = std::bitset<MAX_COMPONENTS>;

/**
 * @brief Base class for all components
 */
class Component {
public:
    virtual ~Component() = default;
    
    // Called when component is first attached to an entity
    virtual void Initialize() {}
    
    // Called when component is removed from an entity
    virtual void Finalize() {}
};

/**
 * @brief Entity class - essentially just an ID with associated components
 */
class Entity {
private:
    EntityID m_id;
    EntityManager* m_manager;
    bool m_active;

public:
    Entity(EntityID id, EntityManager* manager);
    ~Entity() = default;
    
    // Get the entity's unique ID
    EntityID GetID() const { return m_id; }
    
    // Check if entity is active
    bool IsActive() const { return m_active; }
    
    // Activate or deactivate the entity
    void SetActive(bool active) { m_active = active; }
    
    // Add a component to this entity
    template<typename T, typename... Args>
    T* AddComponent(Args&&... args);
    
    // Remove a component from this entity
    template<typename T>
    void RemoveComponent();
    
    // Check if entity has a specific component
    template<typename T>
    bool HasComponent() const;
    
    // Get a component from this entity
    template<typename T>
    T* GetComponent() const;
    
    // Destroy this entity
    void Destroy();
};

/**
 * @brief System base class - processes entities with specific component combinations
 */
class System {
protected:
    EntityManager* m_manager;
    ComponentMask m_componentMask;
    std::set<EntityID> m_entities;
    SystemID m_id;
    bool m_active;

public:
    System(EntityManager* manager);
    virtual ~System() = default;
    
    // Initialize the system
    virtual void Initialize() {}
    
    // Update the system
    virtual void Update(float deltaTime) {}
    
    // Render the system (if applicable)
    virtual void Render() {}
    
    // Called when an entity is added to this system
    virtual void OnEntityAdded(Entity entity) {}
    
    // Called when an entity is removed from this system
    virtual void OnEntityRemoved(Entity entity) {}
    
    // Get the system's unique ID
    SystemID GetID() const { return m_id; }
    
    // Check if system is active
    bool IsActive() const { return m_active; }
    
    // Activate or deactivate the system
    void SetActive(bool active) { m_active = active; }
    
    // Get the component mask for this system
    const ComponentMask& GetComponentMask() const { return m_componentMask; }
    
    // Add a component type requirement to this system
    template<typename T>
    System* RequireComponent();
    
    // Get all entities processed by this system
    const std::set<EntityID>& GetEntities() const { return m_entities; }
    
    // Add an entity to this system
    void AddEntity(EntityID entity);
    
    // Remove an entity from this system
    void RemoveEntity(EntityID entity);
};

/**
 * @brief Manages all entities, components, and systems
 */
class EntityManager {
private:
    // Component type counter for generating unique component IDs
    static ComponentID s_componentTypeCounter;
    
    // System type counter for generating unique system IDs
    static SystemID s_systemTypeCounter;
    
    // Map of entity IDs to component masks
    std::unordered_map<EntityID, ComponentMask> m_entityMasks;
    
    // Map of entity IDs to components
    std::unordered_map<EntityID, std::unordered_map<ComponentID, std::shared_ptr<Component>>> m_entityComponents;
    
    // Queue of entity IDs that are available for reuse
    std::queue<EntityID> m_availableEntityIDs;
    
    // Next entity ID to assign if no recycled IDs are available
    EntityID m_nextEntityID;
    
    // Map of system IDs to systems
    std::unordered_map<SystemID, std::shared_ptr<System>> m_systems;
    
    // Set of entities pending destruction
    std::set<EntityID> m_entitiesToDestroy;

public:
    EntityManager();
    ~EntityManager() = default;
    
    // Create a new entity
    Entity CreateEntity();
    
    // Destroy an entity
    void DestroyEntity(EntityID entity);
    
    // Process pending entity destructions
    void ProcessDestructions();
    
    // Add a component to an entity
    template<typename T, typename... Args>
    T* AddComponent(EntityID entity, Args&&... args);
    
    // Remove a component from an entity
    template<typename T>
    void RemoveComponent(EntityID entity);
    
    // Check if an entity has a specific component
    template<typename T>
    bool HasComponent(EntityID entity) const;
    
    // Get a component from an entity
    template<typename T>
    T* GetComponent(EntityID entity) const;
    
    // Register a system
    template<typename T, typename... Args>
    T* RegisterSystem(Args&&... args);
    
    // Get a system
    template<typename T>
    T* GetSystem() const;
    
    // Update all active systems
    void UpdateSystems(float deltaTime);
    
    // Render all active systems
    void RenderSystems();
    
    // Get the component mask for an entity
    const ComponentMask& GetComponentMask(EntityID entity) const;
    
    // Helper function to get component ID for a type
    template<typename T>
    static ComponentID GetComponentTypeID();
    
    // Helper function to get system ID for a type
    template<typename T>
    static SystemID GetSystemTypeID();
};

// Implementation of Entity methods

inline Entity::Entity(EntityID id, EntityManager* manager)
    : m_id(id), m_manager(manager), m_active(true) {}

template<typename T, typename... Args>
T* Entity::AddComponent(Args&&... args) {
    return m_manager->AddComponent<T>(m_id, std::forward<Args>(args)...);
}

template<typename T>
void Entity::RemoveComponent() {
    m_manager->RemoveComponent<T>(m_id);
}

template<typename T>
bool Entity::HasComponent() const {
    return m_manager->HasComponent<T>(m_id);
}

template<typename T>
T* Entity::GetComponent() const {
    return m_manager->GetComponent<T>(m_id);
}

inline void Entity::Destroy() {
    m_manager->DestroyEntity(m_id);
}

// Implementation of System methods

inline System::System(EntityManager* manager)
    : m_manager(manager), m_id(0), m_active(true) {}

template<typename T>
System* System::RequireComponent() {
    m_componentMask.set(EntityManager::GetComponentTypeID<T>());
    return this;
}

inline void System::AddEntity(EntityID entity) {
    m_entities.insert(entity);
    OnEntityAdded(Entity(entity, m_manager));
}

inline void System::RemoveEntity(EntityID entity) {
    if (m_entities.erase(entity) > 0) {
        OnEntityRemoved(Entity(entity, m_manager));
    }
}

// Implementation of EntityManager methods

// Static member initialization
inline ComponentID EntityManager::s_componentTypeCounter = 0;
inline SystemID EntityManager::s_systemTypeCounter = 0;

inline EntityManager::EntityManager()
    : m_nextEntityID(0) {}

inline Entity EntityManager::CreateEntity() {
    EntityID id;
    
    if (!m_availableEntityIDs.empty()) {
        id = m_availableEntityIDs.front();
        m_availableEntityIDs.pop();
    } else {
        id = m_nextEntityID++;
    }
    
    m_entityMasks[id].reset();
    m_entityComponents[id].clear();
    
    return Entity(id, this);
}

inline void EntityManager::DestroyEntity(EntityID entity) {
    m_entitiesToDestroy.insert(entity);
}

inline void EntityManager::ProcessDestructions() {
    for (EntityID entity : m_entitiesToDestroy) {
        // Remove entity from all systems
        for (auto& systemPair : m_systems) {
            systemPair.second->RemoveEntity(entity);
        }
        
        // Clear entity components
        m_entityComponents.erase(entity);
        m_entityMasks.erase(entity);
        
        // Add entity ID back to available pool
        m_availableEntityIDs.push(entity);
    }
    
    m_entitiesToDestroy.clear();
}

template<typename T, typename... Args>
T* EntityManager::AddComponent(EntityID entity, Args&&... args) {
    const ComponentID componentID = GetComponentTypeID<T>();
    
    // Create the component
    auto component = std::make_shared<T>(std::forward<Args>(args)...);
    m_entityComponents[entity][componentID] = component;
    
    // Update the entity's component mask
    m_entityMasks[entity].set(componentID);
    
    // Initialize the component
    component->Initialize();
    
    // Check if entity should be added to any systems
    for (auto& systemPair : m_systems) {
        const ComponentMask& systemMask = systemPair.second->GetComponentMask();
        
        if ((m_entityMasks[entity] & systemMask) == systemMask) {
            systemPair.second->AddEntity(entity);
        }
    }
    
    return static_cast<T*>(component.get());
}

template<typename T>
void EntityManager::RemoveComponent(EntityID entity) {
    const ComponentID componentID = GetComponentTypeID<T>();
    
    // Check if entity has this component
    if (!m_entityMasks[entity].test(componentID)) {
        return;
    }
    
    // Finalize the component
    auto component = static_cast<T*>(m_entityComponents[entity][componentID].get());
    component->Finalize();
    
    // Remove the component
    m_entityComponents[entity].erase(componentID);
    
    // Update the entity's component mask
    m_entityMasks[entity].reset(componentID);
    
    // Check if entity should be removed from any systems
    for (auto& systemPair : m_systems) {
        const ComponentMask& systemMask = systemPair.second->GetComponentMask();
        
        if ((m_entityMasks[entity] & systemMask) != systemMask) {
            systemPair.second->RemoveEntity(entity);
        }
    }
}

template<typename T>
bool EntityManager::HasComponent(EntityID entity) const {
    const ComponentID componentID = GetComponentTypeID<T>();
    return m_entityMasks.at(entity).test(componentID);
}

template<typename T>
T* EntityManager::GetComponent(EntityID entity) const {
    const ComponentID componentID = GetComponentTypeID<T>();
    
    // Check if entity has this component
    if (!m_entityMasks.at(entity).test(componentID)) {
        return nullptr;
    }
    
    return static_cast<T*>(m_entityComponents.at(entity).at(componentID).get());
}

template<typename T, typename... Args>
T* EntityManager::RegisterSystem(Args&&... args) {
    const SystemID systemID = GetSystemTypeID<T>();
    
    // Create the system
    auto system = std::make_shared<T>(this, std::forward<Args>(args)...);
    system->m_id = systemID;
    m_systems[systemID] = system;
    
    // Initialize the system
    system->Initialize();
    
    // Check existing entities to see if they match this system
    for (const auto& entityPair : m_entityMasks) {
        const EntityID entity = entityPair.first;
        const ComponentMask& entityMask = entityPair.second;
        const ComponentMask& systemMask = system->GetComponentMask();
        
        if ((entityMask & systemMask) == systemMask) {
            system->AddEntity(entity);
        }
    }
    
    return static_cast<T*>(system.get());
}

template<typename T>
T* EntityManager::GetSystem() const {
    const SystemID systemID = GetSystemTypeID<T>();
    
    auto it = m_systems.find(systemID);
    if (it != m_systems.end()) {
        return static_cast<T*>(it->second.get());
    }
    
    return nullptr;
}

inline void EntityManager::UpdateSystems(float deltaTime) {
    for (auto& systemPair : m_systems) {
        if (systemPair.second->IsActive()) {
            systemPair.second->Update(deltaTime);
        }
    }
}

inline void EntityManager::RenderSystems() {
    for (auto& systemPair : m_systems) {
        if (systemPair.second->IsActive()) {
            systemPair.second->Render();
        }
    }
}

inline const ComponentMask& EntityManager::GetComponentMask(EntityID entity) const {
    return m_entityMasks.at(entity);
}

template<typename T>
ComponentID EntityManager::GetComponentTypeID() {
    static const ComponentID typeID = s_componentTypeCounter++;
    return typeID;
}

template<typename T>
SystemID EntityManager::GetSystemTypeID() {
    static const SystemID typeID = s_systemTypeCounter++;
    return typeID;
}

} // namespace CHULUBME