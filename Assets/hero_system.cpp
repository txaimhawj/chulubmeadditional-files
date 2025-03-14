#include "hero_system.h"
#include "../core/engine.h"
#include <json/json.h>
#include <fstream>

namespace CHULUBME {

// HeroComponent implementation
HeroComponent::HeroComponent(const std::string& heroId, const std::string& heroName)
    : m_heroId(heroId)
    , m_heroName(heroName)
    , m_description("")
    , m_role(HeroRole::Fighter)
    , m_difficulty(5)
    , m_level(1)
    , m_experience(0)
    , m_currentHealth(0)
    , m_currentMana(0)
    , m_skinId("")
{
}

void HeroComponent::Initialize() {
    // Initialize current health and mana to max values
    m_currentHealth = m_baseStats.health;
    m_currentMana = m_baseStats.mana;
}

void HeroComponent::Finalize() {
    // Clear abilities
    m_abilities.clear();
    
    // Clear callbacks
    m_damageCallbacks.clear();
    m_healCallbacks.clear();
    m_deathCallbacks.clear();
    m_levelUpCallbacks.clear();
}

void HeroComponent::SetLevel(int level) {
    if (level < 1) level = 1;
    if (level > 18) level = 18; // Max level in most MOBAs
    
    int oldLevel = m_level;
    m_level = level;
    
    // Notify level change
    for (auto& callback : m_levelUpCallbacks) {
        callback(m_level);
    }
}

void HeroComponent::LevelUp() {
    if (m_level < 18) { // Max level in most MOBAs
        m_level++;
        
        // Update health and mana based on level
        float maxHealth = GetCurrentStats().health;
        float maxMana = GetCurrentStats().mana;
        
        // Heal for 30% of missing health and mana on level up
        float missingHealth = maxHealth - m_currentHealth;
        float missingMana = maxMana - m_currentMana;
        
        m_currentHealth += missingHealth * 0.3f;
        m_currentMana += missingMana * 0.3f;
        
        // Notify level up
        for (auto& callback : m_levelUpCallbacks) {
            callback(m_level);
        }
    }
}

HeroStats HeroComponent::GetCurrentStats() const {
    HeroStats currentStats = m_baseStats;
    
    // Apply level-based stat growth
    int levelGrowth = m_level - 1;
    if (levelGrowth > 0) {
        currentStats.health += m_baseStats.healthPerLevel * levelGrowth;
        currentStats.mana += m_baseStats.manaPerLevel * levelGrowth;
        currentStats.attackDamage += m_baseStats.attackDamagePerLevel * levelGrowth;
        currentStats.abilityPower += m_baseStats.abilityPowerPerLevel * levelGrowth;
        currentStats.armor += m_baseStats.armorPerLevel * levelGrowth;
        currentStats.magicResist += m_baseStats.magicResistPerLevel * levelGrowth;
        currentStats.attackSpeed += m_baseStats.attackSpeedPerLevel * levelGrowth;
    }
    
    // TODO: Apply item stats and buffs
    
    return currentStats;
}

void HeroComponent::AddAbility(std::shared_ptr<Ability> ability) {
    m_abilities.push_back(ability);
}

std::shared_ptr<Ability> HeroComponent::GetAbility(int index) const {
    if (index >= 0 && index < m_abilities.size()) {
        return m_abilities[index];
    }
    return nullptr;
}

void HeroComponent::SetSkin(const std::string& skinId) {
    m_skinId = skinId;
    
    // TODO: Load skin model and materials
}

float HeroComponent::TakeDamage(float amount, bool isMagical) {
    if (amount <= 0) return 0;
    
    // Calculate damage reduction based on armor or magic resist
    HeroStats currentStats = GetCurrentStats();
    float damageReduction;
    
    if (isMagical) {
        // Magic damage reduction formula
        damageReduction = currentStats.magicResist / (100.0f + currentStats.magicResist);
    } else {
        // Physical damage reduction formula
        damageReduction = currentStats.armor / (100.0f + currentStats.armor);
    }
    
    // Apply damage reduction
    float actualDamage = amount * (1.0f - damageReduction);
    
    // Apply damage
    m_currentHealth -= actualDamage;
    
    // Check for death
    if (m_currentHealth <= 0) {
        m_currentHealth = 0;
        
        // Notify death
        for (auto& callback : m_deathCallbacks) {
            callback();
        }
    }
    
    // Notify damage
    for (auto& callback : m_damageCallbacks) {
        callback(actualDamage, isMagical);
    }
    
    return actualDamage;
}

void HeroComponent::Heal(float amount) {
    if (amount <= 0) return;
    
    // Calculate max health
    float maxHealth = GetCurrentStats().health;
    
    // Apply healing
    float actualHeal = amount;
    if (m_currentHealth + actualHeal > maxHealth) {
        actualHeal = maxHealth - m_currentHealth;
    }
    
    m_currentHealth += actualHeal;
    
    // Notify healing
    for (auto& callback : m_healCallbacks) {
        callback(actualHeal);
    }
}

bool HeroComponent::UseMana(float amount) {
    if (amount <= 0) return true;
    
    // Check if enough mana
    if (m_currentMana < amount) {
        return false;
    }
    
    // Use mana
    m_currentMana -= amount;
    return true;
}

void HeroComponent::RestoreMana(float amount) {
    if (amount <= 0) return;
    
    // Calculate max mana
    float maxMana = GetCurrentStats().mana;
    
    // Apply mana restoration
    float actualRestore = amount;
    if (m_currentMana + actualRestore > maxMana) {
        actualRestore = maxMana - m_currentMana;
    }
    
    m_currentMana += actualRestore;
}

void HeroComponent::RegisterDamageCallback(std::function<void(float, bool)> callback) {
    m_damageCallbacks.push_back(callback);
}

void HeroComponent::RegisterHealCallback(std::function<void(float)> callback) {
    m_healCallbacks.push_back(callback);
}

void HeroComponent::RegisterDeathCallback(std::function<void()> callback) {
    m_deathCallbacks.push_back(callback);
}

void HeroComponent::RegisterLevelUpCallback(std::function<void(int)> callback) {
    m_levelUpCallbacks.push_back(callback);
}

// Ability implementation
Ability::Ability(const std::string& abilityId, const std::string& abilityName)
    : m_abilityId(abilityId)
    , m_abilityName(abilityName)
    , m_description("")
    , m_cooldown(0.0f)
    , m_cooldownRemaining(0.0f)
    , m_manaCost(0.0f)
    , m_range(0.0f)
    , m_level(1)
    , m_abilityType(AbilityType::Targeted)
    , m_owner(0, nullptr)
{
}

void Ability::Initialize() {
    // Reset cooldown
    m_cooldownRemaining = 0.0f;
}

void Ability::LevelUp() {
    if (m_level < 5) { // Most abilities max at level 5
        m_level++;
    }
}

void Ability::Update(float deltaTime) {
    // Update cooldown
    if (m_cooldownRemaining > 0.0f) {
        m_cooldownRemaining -= deltaTime;
        if (m_cooldownRemaining < 0.0f) {
            m_cooldownRemaining = 0.0f;
        }
    }
}

bool Ability::Use(Entity caster, Entity target) {
    // Check if ability is ready
    if (!IsReady()) {
        return false;
    }
    
    // Check if caster has enough mana
    HeroComponent* heroComponent = caster.GetComponent<HeroComponent>();
    if (heroComponent && !heroComponent->UseMana(m_manaCost)) {
        return false;
    }
    
    // Start cooldown
    m_cooldownRemaining = m_cooldown;
    
    return true;
}

bool Ability::UseAtPosition(Entity caster, const glm::vec3& position) {
    // Default implementation just calls Use with no target
    return Use(caster);
}

bool Ability::UseInDirection(Entity caster, const glm::vec3& direction) {
    // Default implementation just calls Use with no target
    return Use(caster);
}

// HeroSystem implementation
HeroSystem::HeroSystem(EntityManager* manager)
    : System(manager)
{
    RequireComponent<HeroComponent>();
}

void HeroSystem::Initialize() {
    // Nothing to do here
}

void HeroSystem::Update(float deltaTime) {
    // Update all heroes
    for (EntityID entityID : GetEntities()) {
        Entity entity(entityID, m_manager);
        HeroComponent* heroComponent = entity.GetComponent<HeroComponent>();
        
        // Update hero logic here
        // For example, health and mana regeneration
        if (heroComponent) {
            HeroStats currentStats = heroComponent->GetCurrentStats();
            heroComponent->Heal(currentStats.healthRegen * deltaTime);
            heroComponent->RestoreMana(currentStats.manaRegen * deltaTime);
        }
    }
}

void HeroSystem::OnEntityAdded(Entity entity) {
    // Add hero to map
    HeroComponent* heroComponent = entity.GetComponent<HeroComponent>();
    if (heroComponent) {
        m_heroes[heroComponent->GetHeroID()] = entity.GetID();
    }
}

void HeroSystem::OnEntityRemoved(Entity entity) {
    // Remove hero from map
    HeroComponent* heroComponent = entity.GetComponent<HeroComponent>();
    if (heroComponent) {
        m_heroes.erase(heroComponent->GetHeroID());
    }
}

Entity HeroSystem::CreateHero(const std::string& heroId, const std::string& heroName) {
    // Create entity
    Entity entity = m_manager->CreateEntity();
    
    // Add hero component
    HeroComponent* heroComponent = entity.AddComponent<HeroComponent>(heroId, heroName);
    
    // Add transform component
    entity.AddComponent<Transform>();
    
    // Add mesh renderer component
    entity.AddComponent<MeshRenderer>();
    
    return entity;
}

Entity HeroSystem::GetHeroByID(const std::string& heroId) const {
    auto it = m_heroes.find(heroId);
    if (it != m_heroes.end()) {
        return Entity(it->second, m_manager);
    }
    return Entity(0, nullptr);
}

std::vector<Entity> HeroSystem::GetAllHeroes() const {
    std::vector<Entity> heroes;
    for (const auto& pair : m_heroes) {
        heroes.push_back(Entity(pair.second, m_manager));
    }
    return heroes;
}

bool HeroSystem::LoadHeroData(const std::string& filename) {
    // Open file
    std::ifstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    // Parse JSON
    Json::Value root;
    file >> root;
    
    // Load heroes
    const Json::Value& heroes = root["heroes"];
    for (const auto& heroData : heroes) {
        // Create hero
        std::string heroId = heroData["id"].asString();
        std::string heroName = heroData["name"].asString();
        
        Entity hero = CreateHero(heroId, heroName);
        HeroComponent* heroComponent = hero.GetComponent<HeroComponent>();
        
        // Set hero properties
        heroComponent->SetDescription(heroData["description"].asString());
        heroComponent->SetRole(static_cast<HeroComponent::HeroRole>(heroData["role"].asInt()));
        heroComponent->SetDifficulty(heroData["difficulty"].asInt());
        
        // Set hero stats
        HeroStats stats;
        const Json::Value& statsData = heroData["stats"];
        stats.health = statsData["health"].asFloat();
        stats.mana = statsData["mana"].asFloat();
        stats.attackDamage = statsData["attackDamage"].asFloat();
        stats.abilityPower = statsData["abilityPower"].asFloat();
        stats.armor = statsData["armor"].asFloat();
        stats.magicResist = statsData["magicResist"].asFloat();
        stats.attackSpeed = statsData["attackSpeed"].asFloat();
        stats.movementSpeed = statsData["movementSpeed"].asFloat();
        stats.healthRegen = statsData["healthRegen"].asFloat();
        stats.manaRegen = statsData["manaRegen"].asFloat();
        stats.critChance = statsData["critChance"].asFloat();
        stats.critDamage = statsData["critDamage"].asFloat();
        stats.lifeSteal = statsData["lifeSteal"].asFloat();
        stats.cooldownReduction = statsData["cooldownReduction"].asFloat();
        
        stats.healthPerLevel = statsData["healthPerLevel"].asFloat();
        stats.manaPerLevel = statsData["manaPerLevel"].asFloat();
        stats.attackDamagePerLevel = statsData["attackDamagePerLevel"].asFloat();
        stats.abilityPowerPerLevel = statsData["abilityPowerPerLevel"].asFloat();
        stats.armorPerLevel = statsData["armorPerLevel"].asFloat();
        stats.magicResistPerLevel = statsData["magicResistPerLevel"].asFloat();
        stats.attackSpeedPerLevel = statsData["attackSpeedPerLevel"].asFloat();
        
        heroComponent->SetBaseStats(stats);
        
        // TODO: Load abilities
    }
    
    return true;
}

bool HeroSystem::SaveHeroData(const std::string& filename) const {
    // Create root JSON object
    Json::Value root;
    Json::Value heroesArray(Json::arrayValue);
    
    // Save heroes
    for (const auto& pair : m_heroes) {
        Entity entity(pair.second, m_manager);
        HeroComponent* heroComponent = entity.GetComponent<HeroComponent>();
        
        if (heroComponent) {
            Json::Value heroData;
            heroData["id"] = heroComponent->GetHeroID();
            heroData["name"] = heroComponent->GetHeroName();
            heroData["description"] = heroComponent->GetDescription();
            heroData["role"] = static_cast<int>(heroComponent->GetRole());
            heroData["difficulty"] = heroComponent->GetDifficulty();
            
            // Save stats
            Json::Value statsData;
            const HeroStats& stats = heroComponent->GetBaseStats();
            statsData["health"] = stats.health;
            statsData["mana"] = stats.mana;
            statsData["attackDamage"] = stats.attackDamage;
            statsData["abilityPower"] = stats.abilityPower;
            statsData["armor"] = stats.armor;
            statsData["magicResist"] = stats.magicResist;
            statsData["attackSpeed"] = stats.attackSpeed;
            statsData["movementSpeed"] = stats.movementSpeed;
            statsData["healthRegen"] = stats.healthRegen;
            statsData["manaRegen"] = stats.manaRegen;
            statsData["critChance"] = stats.critChance;
            statsData["critDamage"] = stats.critDamage;
            statsData["lifeSteal"] = stats.lifeSteal;
            statsData["cooldownReduction"] = stats.cooldownReduction;
            
            statsData["healthPerLevel"] = stats.healthPerLevel;
            statsData["manaPerLevel"] = stats.manaPerLevel;
            statsData["attackDamagePerLevel"] = stats.attackDamagePerLevel;
            statsData["abilityPowerPerLevel"] = stats.abilityPowerPerLevel;
            statsData["armorPerLevel"] = stats.armorPerLevel;
            statsData["magicResistPerLevel"] = stats.magicResistPerLevel;
            statsData["attackSpeedPerLevel"] = stats.attackSpeedPerLevel;
            
            heroData["stats"] = statsData;
            
            // TODO: Save abilities
            
            heroesArray.append(heroData);
        }
    }
    
    root["heroes"] = heroesArray;
    
    // Write to file
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    file << root;
    return true;
}

// AbilitySystem implementation
AbilitySystem::AbilitySystem(EntityManager* manager)
    : System(manager)
{
    RequireComponent<HeroComponent>();
}

void AbilitySystem::Initialize() {
    // Nothing to do here
}

void AbilitySystem::Update(float deltaTime) {
    // Update all abilities
    for (EntityID entityID : GetEntities()) {
        Entity entity(entityID, m_manager);
        HeroComponent* heroComponent = entity.GetComponent<HeroComponent>();
        
        if (heroComponent) {
            // Update all abilities for this hero
            for (auto& ability : heroComponent->GetAbilities()) {
                ability->Update(deltaTime);
            }
        }
    }
}

void AbilitySystem::OnEntityAdded(Entity entity) {
    // Nothing to do here
}

void AbilitySystem::OnEntityRemoved(Entity entity) {
    // Nothing to do here
}

std::shared_ptr<Ability> AbilitySystem::CreateAbility(const std::string& typeName, const std::string& abilityId, const std::string& abilityName) {
    auto it = m_abilityFactories.find(typeName);
    if (it != m_abilityFactories.end()) {
        std::shared_ptr<Ability> ability = it->second();
        ability->SetAbilityID(abilityId);
        ability->SetAbilityName(abilityName);
        ability->Initialize();
        
        // Store ability
        m_abilities[abilityId] = ability;
        
        return ability;
    }
 <response clipped><NOTE>To save on context only part of this file has been shown to you. You should retry this tool after you have searched inside the file with `grep -n` in order to find the line numbers of what you are looking for.</NOTE>