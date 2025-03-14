#pragma once

#include "hero_system.h"
#include <glm/glm.hpp>

namespace CHULUBME {

/**
 * @brief Base class for targeted abilities
 */
class TargetedAbility : public Ability {
public:
    TargetedAbility(const std::string& abilityId = "", const std::string& abilityName = "");
    virtual ~TargetedAbility() = default;
    
    // Initialize the ability
    void Initialize() override;
    
    // Use the ability on a target
    bool Use(Entity caster, Entity target = Entity(0, nullptr)) override;
    
    // Set damage
    void SetDamage(float baseDamage, float damagePerLevel, float apRatio, float adRatio);
    
    // Get damage for current level
    float GetDamage() const;
    
    // Set whether this is magical damage
    void SetMagicalDamage(bool isMagical) { m_isMagicalDamage = isMagical; }
    
    // Check if this is magical damage
    bool IsMagicalDamage() const { return m_isMagicalDamage; }
    
protected:
    // Apply ability effects to target
    virtual bool ApplyEffects(Entity caster, Entity target);
    
    // Damage parameters
    float m_baseDamage;
    float m_damagePerLevel;
    float m_apRatio;
    float m_adRatio;
    bool m_isMagicalDamage;
};

/**
 * @brief Base class for area of effect abilities
 */
class AreaOfEffectAbility : public Ability {
public:
    AreaOfEffectAbility(const std::string& abilityId = "", const std::string& abilityName = "");
    virtual ~AreaOfEffectAbility() = default;
    
    // Initialize the ability
    void Initialize() override;
    
    // Use the ability at a position
    bool UseAtPosition(Entity caster, const glm::vec3& position) override;
    
    // Set radius
    void SetRadius(float radius) { m_radius = radius; }
    
    // Get radius
    float GetRadius() const { return m_radius; }
    
    // Set damage
    void SetDamage(float baseDamage, float damagePerLevel, float apRatio, float adRatio);
    
    // Get damage for current level
    float GetDamage() const;
    
    // Set whether this is magical damage
    void SetMagicalDamage(bool isMagical) { m_isMagicalDamage = isMagical; }
    
    // Check if this is magical damage
    bool IsMagicalDamage() const { return m_isMagicalDamage; }
    
protected:
    // Apply ability effects to targets in area
    virtual bool ApplyEffects(Entity caster, const glm::vec3& position, const std::vector<Entity>& targets);
    
    // Find targets in area
    std::vector<Entity> FindTargetsInArea(const glm::vec3& position) const;
    
    // Area parameters
    float m_radius;
    
    // Damage parameters
    float m_baseDamage;
    float m_damagePerLevel;
    float m_apRatio;
    float m_adRatio;
    bool m_isMagicalDamage;
};

/**
 * @brief Base class for skillshot abilities
 */
class SkillshotAbility : public Ability {
public:
    SkillshotAbility(const std::string& abilityId = "", const std::string& abilityName = "");
    virtual ~SkillshotAbility() = default;
    
    // Initialize the ability
    void Initialize() override;
    
    // Use the ability in a direction
    bool UseInDirection(Entity caster, const glm::vec3& direction) override;
    
    // Set width
    void SetWidth(float width) { m_width = width; }
    
    // Get width
    float GetWidth() const { return m_width; }
    
    // Set speed
    void SetSpeed(float speed) { m_speed = speed; }
    
    // Get speed
    float GetSpeed() const { return m_speed; }
    
    // Set damage
    void SetDamage(float baseDamage, float damagePerLevel, float apRatio, float adRatio);
    
    // Get damage for current level
    float GetDamage() const;
    
    // Set whether this is magical damage
    void SetMagicalDamage(bool isMagical) { m_isMagicalDamage = isMagical; }
    
    // Check if this is magical damage
    bool IsMagicalDamage() const { return m_isMagicalDamage; }
    
protected:
    // Apply ability effects to targets hit by skillshot
    virtual bool ApplyEffects(Entity caster, const glm::vec3& origin, const glm::vec3& direction, const std::vector<Entity>& targets);
    
    // Find targets hit by skillshot
    std::vector<Entity> FindTargetsInSkillshot(const glm::vec3& origin, const glm::vec3& direction) const;
    
    // Skillshot parameters
    float m_width;
    float m_speed;
    
    // Damage parameters
    float m_baseDamage;
    float m_damagePerLevel;
    float m_apRatio;
    float m_adRatio;
    bool m_isMagicalDamage;
};

/**
 * @brief Base class for self buff abilities
 */
class SelfBuffAbility : public Ability {
public:
    SelfBuffAbility(const std::string& abilityId = "", const std::string& abilityName = "");
    virtual ~SelfBuffAbility() = default;
    
    // Initialize the ability
    void Initialize() override;
    
    // Use the ability
    bool Use(Entity caster, Entity target = Entity(0, nullptr)) override;
    
    // Set duration
    void SetDuration(float duration) { m_duration = duration; }
    
    // Get duration
    float GetDuration() const { return m_duration; }
    
    // Update the ability
    void Update(float deltaTime) override;
    
protected:
    // Apply buff effects to caster
    virtual bool ApplyBuffEffects(Entity caster);
    
    // Remove buff effects from caster
    virtual void RemoveBuffEffects(Entity caster);
    
    // Buff parameters
    float m_duration;
    float m_remainingDuration;
    bool m_isActive;
    Entity m_activeCaster;
};

/**
 * @brief Damage ability that deals damage to a target
 */
class DamageAbility : public TargetedAbility {
public:
    DamageAbility(const std::string& abilityId = "", const std::string& abilityName = "");
    virtual ~DamageAbility() = default;
    
    // Initialize the ability
    void Initialize() override;
    
protected:
    // Apply ability effects to target
    bool ApplyEffects(Entity caster, Entity target) override;
};

/**
 * @brief Heal ability that heals a target
 */
class HealAbility : public TargetedAbility {
public:
    HealAbility(const std::string& abilityId = "", const std::string& abilityName = "");
    virtual ~HealAbility() = default;
    
    // Initialize the ability
    void Initialize() override;
    
    // Set healing
    void SetHealing(float baseHealing, float healingPerLevel, float apRatio);
    
    // Get healing for current level
    float GetHealing() const;
    
protected:
    // Apply ability effects to target
    bool ApplyEffects(Entity caster, Entity target) override;
    
    // Healing parameters
    float m_baseHealing;
    float m_healingPerLevel;
    float m_apRatio;
};

/**
 * @brief Area damage ability that deals damage to targets in an area
 */
class AreaDamageAbility : public AreaOfEffectAbility {
public:
    AreaDamageAbility(const std::string& abilityId = "", const std::string& abilityName = "");
    virtual ~AreaDamageAbility() = default;
    
    // Initialize the ability
    void Initialize() override;
    
protected:
    // Apply ability effects to targets in area
    bool ApplyEffects(Entity caster, const glm::vec3& position, const std::vector<Entity>& targets) override;
};

/**
 * @brief Skillshot damage ability that deals damage to targets hit by a skillshot
 */
class SkillshotDamageAbility : public SkillshotAbility {
public:
    SkillshotDamageAbility(const std::string& abilityId = "", const std::string& abilityName = "");
    virtual ~SkillshotDamageAbility() = default;
    
    // Initialize the ability
    void Initialize() override;
    
protected:
    // Apply ability effects to targets hit by skillshot
    bool ApplyEffects(Entity caster, const glm::vec3& origin, const glm::vec3& direction, const std::vector<Entity>& targets) override;
};

/**
 * @brief Movement speed buff ability that increases the caster's movement speed
 */
class MovementSpeedBuffAbility : public SelfBuffAbility {
public:
    MovementSpeedBuffAbility(const std::string& abilityId = "", const std::string& abilityName = "");
    virtual ~MovementSpeedBuffAbility() = default;
    
    // Initialize the ability
    void Initialize() override;
    
    // Set movement speed bonus
    void SetMovementSpeedBonus(float bonus) { m_movementSpeedBonus = bonus; }
    
    // Get movement speed bonus
    float GetMovementSpeedBonus() const { return m_movementSpeedBonus; }
    
protected:
    // Apply buff effects to caster
    bool ApplyBuffEffects(Entity caster) override;
    
    // Remove buff effects from caster
    void RemoveBuffEffects(Entity caster) override;
    
    // Buff parameters
    float m_movementSpeedBonus;
};

/**
 * @brief Attack damage buff ability that increases the caster's attack damage
 */
class AttackDamageBuffAbility : public SelfBuffAbility {
public:
    AttackDamageBuffAbility(const std::string& abilityId = "", const std::string& abilityName = "");
    virtual ~AttackDamageBuffAbility() = default;
    
    // Initialize the ability
    void Initialize() override;
    
    // Set attack damage bonus
    void SetAttackDamageBonus(float bonus) { m_attackDamageBonus = bonus; }
    
    // Get attack damage bonus
    float GetAttackDamageBonus() const { return m_attackDamageBonus; }
    
protected:
    // Apply buff effects to caster
    bool ApplyBuffEffects(Entity caster) override;
    
    // Remove buff effects from caster
    void RemoveBuffEffects(Entity caster) override;
    
    // Buff parameters
    float m_attackDamageBonus;
};

} // namespace CHULUBME
