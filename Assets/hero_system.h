#pragma once

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <unordered_map>

#include "../core/ecs.h"
#include "../rendering/renderer.h"

namespace CHULUBME {

// Forward declarations
class Ability;
class AbilitySystem;

/**
 * @brief Stats for heroes
 */
struct HeroStats {
    // Base stats
    float health;
    float mana;
    float attackDamage;
    float abilityPower;
    float armor;
    float magicResist;
    float attackSpeed;
    float movementSpeed;
    float healthRegen;
    float manaRegen;
    float critChance;
    float critDamage;
    float lifeSteal;
    float cooldownReduction;
    
    // Level-based stat growth
    float healthPerLevel;
    float manaPerLevel;
    float attackDamagePerLevel;
    float abilityPowerPerLevel;
    float armorPerLevel;
    float magicResistPerLevel;
    float attackSpeedPerLevel;
    
    // Constructor with default values
    HeroStats() :
        health(600.0f),
        mana(300.0f),
        attackDamage(60.0f),
        abilityPower(0.0f),
        armor(30.0f),
        magicResist(30.0f),
        attackSpeed(0.7f),
        movementSpeed(350.0f),
        healthRegen(5.0f),
        manaRegen(3.0f),
        critChance(0.0f),
        critDamage(2.0f),
        lifeSteal(0.0f),
        cooldownReduction(0.0f),
        healthPerLevel(90.0f),
        manaPerLevel(40.0f),
        attackDamagePerLevel(3.0f),
        abilityPowerPerLevel(0.0f),
        armorPerLevel(3.5f),
        magicResistPerLevel(1.25f),
        attackSpeedPerLevel(0.02f)
    {}
};

/**
 * @brief Hero component for MOBA heroes
 */
class HeroComponent : public Component {
public:
    HeroComponent(const std::string& heroId = "", const std::string& heroName = "");
    ~HeroComponent() = default;
    
    // Initialize the hero component
    void Initialize() override;
    
    // Finalize the hero component
    void Finalize() override;
    
    // Set hero ID
    void SetHeroID(const std::string& heroId) { m_heroId = heroId; }
    
    // Get hero ID
    std::string GetHeroID() const { return m_heroId; }
    
    // Set hero name
    void SetHeroName(const std::string& heroName) { m_heroName = heroName; }
    
    // Get hero name
    std::string GetHeroName() const { return m_heroName; }
    
    // Set hero description
    void SetDescription(const std::string& description) { m_description = description; }
    
    // Get hero description
    std::string GetDescription() const { return m_description; }
    
    // Set hero role
    enum class HeroRole {
        Tank,
        Fighter,
        Assassin,
        Mage,
        Marksman,
        Support
    };
    void SetRole(HeroRole role) { m_role = role; }
    
    // Get hero role
    HeroRole GetRole() const { return m_role; }
    
    // Set hero difficulty (1-10)
    void SetDifficulty(int difficulty) { m_difficulty = difficulty; }
    
    // Get hero difficulty
    int GetDifficulty() const { return m_difficulty; }
    
    // Set hero level
    void SetLevel(int level);
    
    // Get hero level
    int GetLevel() const { return m_level; }
    
    // Level up the hero
    void LevelUp();
    
    // Set base stats
    void SetBaseStats(const HeroStats& stats) { m_baseStats = stats; }
    
    // Get base stats
    const HeroStats& GetBaseStats() const { return m_baseStats; }
    
    // Get current stats (base + items + buffs)
    HeroStats GetCurrentStats() const;
    
    // Add ability
    void AddAbility(std::shared_ptr<Ability> ability);
    
    // Get ability by index
    std::shared_ptr<Ability> GetAbility(int index) const;
    
    // Get all abilities
    const std::vector<std::shared_ptr<Ability>>& GetAbilities() const { return m_abilities; }
    
    // Set hero model
    void SetModel(std::shared_ptr<Mesh> model) { m_model = model; }
    
    // Get hero model
    std::shared_ptr<Mesh> GetModel() const { return m_model; }
    
    // Set hero material
    void SetMaterial(std::shared_ptr<Material> material) { m_material = material; }
    
    // Get hero material
    std::shared_ptr<Material> GetMaterial() const { return m_material; }
    
    // Set hero portrait
    void SetPortrait(std::shared_ptr<Texture> portrait) { m_portrait = portrait; }
    
    // Get hero portrait
    std::shared_ptr<Texture> GetPortrait() const { return m_portrait; }
    
    // Set hero skin (NFT)
    void SetSkin(const std::string& skinId);
    
    // Get hero skin ID
    std::string GetSkinID() const { return m_skinId; }
    
    // Take damage
    float TakeDamage(float amount, bool isMagical = false);
    
    // Heal
    void Heal(float amount);
    
    // Use mana
    bool UseMana(float amount);
    
    // Restore mana
    void RestoreMana(float amount);
    
    // Register damage callback
    void RegisterDamageCallback(std::function<void(float, bool)> callback);
    
    // Register heal callback
    void RegisterHealCallback(std::function<void(float)> callback);
    
    // Register death callback
    void RegisterDeathCallback(std::function<void()> callback);
    
    // Register level up callback
    void RegisterLevelUpCallback(std::function<void(int)> callback);
    
private:
    // Hero identification
    std::string m_heroId;
    std::string m_heroName;
    std::string m_description;
    HeroRole m_role;
    int m_difficulty;
    
    // Hero level and experience
    int m_level;
    int m_experience;
    
    // Hero stats
    HeroStats m_baseStats;
    float m_currentHealth;
    float m_currentMana;
    
    // Hero abilities
    std::vector<std::shared_ptr<Ability>> m_abilities;
    
    // Hero visuals
    std::shared_ptr<Mesh> m_model;
    std::shared_ptr<Material> m_material;
    std::shared_ptr<Texture> m_portrait;
    std::string m_skinId;
    
    // Callbacks
    std::vector<std::function<void(float, bool)>> m_damageCallbacks;
    std::vector<std::function<void(float)>> m_healCallbacks;
    std::vector<std::function<void()>> m_deathCallbacks;
    std::vector<std::function<void(int)>> m_levelUpCallbacks;
};

/**
 * @brief Ability base class for hero abilities
 */
class Ability {
public:
    Ability(const std::string& abilityId = "", const std::string& abilityName = "");
    virtual ~Ability() = default;
    
    // Initialize the ability
    virtual void Initialize();
    
    // Set ability ID
    void SetAbilityID(const std::string& abilityId) { m_abilityId = abilityId; }
    
    // Get ability ID
    std::string GetAbilityID() const { return m_abilityId; }
    
    // Set ability name
    void SetAbilityName(const std::string& abilityName) { m_abilityName = abilityName; }
    
    // Get ability name
    std::string GetAbilityName() const { return m_abilityName; }
    
    // Set ability description
    void SetDescription(const std::string& description) { m_description = description; }
    
    // Get ability description
    std::string GetDescription() const { return m_description; }
    
    // Set ability icon
    void SetIcon(std::shared_ptr<Texture> icon) { m_icon = icon; }
    
    // Get ability icon
    std::shared_ptr<Texture> GetIcon() const { return m_icon; }
    
    // Set ability cooldown
    void SetCooldown(float cooldown) { m_cooldown = cooldown; }
    
    // Get ability cooldown
    float GetCooldown() const { return m_cooldown; }
    
    // Get current cooldown remaining
    float GetCooldownRemaining() const { return m_cooldownRemaining; }
    
    // Set ability mana cost
    void SetManaCost(float manaCost) { m_manaCost = manaCost; }
    
    // Get ability mana cost
    float GetManaCost() const { return m_manaCost; }
    
    // Set ability range
    void SetRange(float range) { m_range = range; }
    
    // Get ability range
    float GetRange() const { return m_range; }
    
    // Set ability level
    void SetLevel(int level) { m_level = level; }
    
    // Get ability level
    int GetLevel() const { return m_level; }
    
    // Level up the ability
    virtual void LevelUp();
    
    // Check if ability is ready to use
    bool IsReady() const { return m_cooldownRemaining <= 0.0f; }
    
    // Update ability (cooldowns, etc.)
    virtual void Update(float deltaTime);
    
    // Use the ability
    virtual bool Use(Entity caster, Entity target = Entity(0, nullptr));
    
    // Use the ability at a position
    virtual bool UseAtPosition(Entity caster, const glm::vec3& position);
    
    // Use the ability in a direction
    virtual bool UseInDirection(Entity caster, const glm::vec3& direction);
    
    // Get ability type
    enum class AbilityType {
        Passive,
        Targeted,
        AreaOfEffect,
        Skillshot,
        SelfBuff
    };
    AbilityType GetAbilityType() const { return m_abilityType; }
    
protected:
    // Ability identification
    std::string m_abilityId;
    std::string m_abilityName;
    std::string m_description;
    
    // Ability visuals
    std::shared_ptr<Texture> m_icon;
    
    // Ability parameters
    float m_cooldown;
    float m_cooldownRemaining;
    float m_manaCost;
    float m_range;
    int m_level;
    AbilityType m_abilityType;
    
    // Owner reference
    Entity m_owner;
};

/**
 * @brief Hero system for managing heroes
 */
class HeroSystem : public System {
public:
    HeroSystem(EntityManager* manager);
    ~HeroSystem() = default;
    
    // Initialize the hero system
    void Initialize() override;
    
    // Update the hero system
    void Update(float deltaTime) override;
    
    // Called when an entity is added to this system
    void OnEntityAdded(Entity entity) override;
    
    // Called when an entity is removed from this system
    void OnEntityRemoved(Entity entity) override;
    
    // Create a new hero
    Entity CreateHero(const std::string& heroId, const std::string& heroName);
    
    // Get a hero by ID
    Entity GetHeroByID(const std::string& heroId) const;
    
    // Get all heroes
    std::vector<Entity> GetAllHeroes() const;
    
    // Load hero data from file
    bool LoadHeroData(const std::string& filename);
    
    // Save hero data to file
    bool SaveHeroData(const std::string& filename) const;
    
private:
    // Map of hero IDs to entities
    std::unordered_map<std::string, EntityID> m_heroes;
};

/**
 * @brief Ability system for managing abilities
 */
class AbilitySystem : public System {
public:
    AbilitySystem(EntityManager* manager);
    ~AbilitySystem() = default;
    
    // Initialize the ability system
    void Initialize() override;
    
    // Update the ability system
    void Update(float deltaTime) override;
    
    // Called when an entity is added to this system
    void OnEntityAdded(Entity entity) override;
    
    // Called when an entity is removed from this system
    void OnEntityRemoved(Entity entity) override;
    
    // Register an ability type
    template<typename T>
    void RegisterAbilityType(const std::string& typeName);
    
    // Create an ability by type name
    std::shared_ptr<Ability> CreateAbility(const std::string& typeName, const std::string& abilityId, const std::string& abilityName);
    
    // Load ability data from file
    bool LoadAbilityData(const std::string& filename);
    
    // Save ability data to file
    bool SaveAbilityData(const std::string& filename) const;
    
private:
    // Map of ability type names to factory functions
    std::unordered_map<std::string, std::function<std::shared_ptr<Ability>()>> m_abilityFactories;
    
    // Map of ability IDs to abilities
    std::unordered_map<std::string, std::shared_ptr<Ability>> m_abilities;
};

// Template implementation for registering ability types
template<typename T>
void AbilitySystem::RegisterAbilityType(const std::string& typeName) {
    m_abilityFactories[typeName] = []() { return std::make_shared<T>(); };
}

} // namespace CHULUBME
