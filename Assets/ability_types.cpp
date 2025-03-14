#include "ability_types.h"
#include "../core/engine.h"

namespace CHULUBME {

// TargetedAbility implementation
TargetedAbility::TargetedAbility(const std::string& abilityId, const std::string& abilityName)
    : Ability(abilityId, abilityName)
    , m_baseDamage(0.0f)
    , m_damagePerLevel(0.0f)
    , m_apRatio(0.0f)
    , m_adRatio(0.0f)
    , m_isMagicalDamage(true)
{
    m_abilityType = AbilityType::Targeted;
}

void TargetedAbility::Initialize() {
    Ability::Initialize();
}

bool TargetedAbility::Use(Entity caster, Entity target) {
    if (!Ability::Use(caster, target)) {
        return false;
    }
    
    // Check if target is valid
    if (!target.IsActive()) {
        return false;
    }
    
    // Apply effects to target
    return ApplyEffects(caster, target);
}

void TargetedAbility::SetDamage(float baseDamage, float damagePerLevel, float apRatio, float adRatio) {
    m_baseDamage = baseDamage;
    m_damagePerLevel = damagePerLevel;
    m_apRatio = apRatio;
    m_adRatio = adRatio;
}

float TargetedAbility::GetDamage() const {
    return m_baseDamage + (m_damagePerLevel * (m_level - 1));
}

bool TargetedAbility::ApplyEffects(Entity caster, Entity target) {
    // Default implementation does nothing
    return true;
}

// AreaOfEffectAbility implementation
AreaOfEffectAbility::AreaOfEffectAbility(const std::string& abilityId, const std::string& abilityName)
    : Ability(abilityId, abilityName)
    , m_radius(0.0f)
    , m_baseDamage(0.0f)
    , m_damagePerLevel(0.0f)
    , m_apRatio(0.0f)
    , m_adRatio(0.0f)
    , m_isMagicalDamage(true)
{
    m_abilityType = AbilityType::AreaOfEffect;
}

void AreaOfEffectAbility::Initialize() {
    Ability::Initialize();
}

bool AreaOfEffectAbility::UseAtPosition(Entity caster, const glm::vec3& position) {
    if (!Ability::Use(caster)) {
        return false;
    }
    
    // Find targets in area
    std::vector<Entity> targets = FindTargetsInArea(position);
    
    // Apply effects to targets
    return ApplyEffects(caster, position, targets);
}

void AreaOfEffectAbility::SetDamage(float baseDamage, float damagePerLevel, float apRatio, float adRatio) {
    m_baseDamage = baseDamage;
    m_damagePerLevel = damagePerLevel;
    m_apRatio = apRatio;
    m_adRatio = adRatio;
}

float AreaOfEffectAbility::GetDamage() const {
    return m_baseDamage + (m_damagePerLevel * (m_level - 1));
}

bool AreaOfEffectAbility::ApplyEffects(Entity caster, const glm::vec3& position, const std::vector<Entity>& targets) {
    // Default implementation does nothing
    return true;
}

std::vector<Entity> AreaOfEffectAbility::FindTargetsInArea(const glm::vec3& position) const {
    // TODO: Implement proper target finding using physics system
    // For now, just return an empty vector
    return std::vector<Entity>();
}

// SkillshotAbility implementation
SkillshotAbility::SkillshotAbility(const std::string& abilityId, const std::string& abilityName)
    : Ability(abilityId, abilityName)
    , m_width(0.0f)
    , m_speed(0.0f)
    , m_baseDamage(0.0f)
    , m_damagePerLevel(0.0f)
    , m_apRatio(0.0f)
    , m_adRatio(0.0f)
    , m_isMagicalDamage(true)
{
    m_abilityType = AbilityType::Skillshot;
}

void SkillshotAbility::Initialize() {
    Ability::Initialize();
}

bool SkillshotAbility::UseInDirection(Entity caster, const glm::vec3& direction) {
    if (!Ability::Use(caster)) {
        return false;
    }
    
    // Get caster position
    Transform* transform = caster.GetComponent<Transform>();
    if (!transform) {
        return false;
    }
    
    glm::vec3 origin = transform->GetPosition();
    
    // Find targets hit by skillshot
    std::vector<Entity> targets = FindTargetsInSkillshot(origin, direction);
    
    // Apply effects to targets
    return ApplyEffects(caster, origin, direction, targets);
}

void SkillshotAbility::SetDamage(float baseDamage, float damagePerLevel, float apRatio, float adRatio) {
    m_baseDamage = baseDamage;
    m_damagePerLevel = damagePerLevel;
    m_apRatio = apRatio;
    m_adRatio = adRatio;
}

float SkillshotAbility::GetDamage() const {
    return m_baseDamage + (m_damagePerLevel * (m_level - 1));
}

bool SkillshotAbility::ApplyEffects(Entity caster, const glm::vec3& origin, const glm::vec3& direction, const std::vector<Entity>& targets) {
    // Default implementation does nothing
    return true;
}

std::vector<Entity> SkillshotAbility::FindTargetsInSkillshot(const glm::vec3& origin, const glm::vec3& direction) const {
    // TODO: Implement proper target finding using physics system
    // For now, just return an empty vector
    return std::vector<Entity>();
}

// SelfBuffAbility implementation
SelfBuffAbility::SelfBuffAbility(const std::string& abilityId, const std::string& abilityName)
    : Ability(abilityId, abilityName)
    , m_duration(0.0f)
    , m_remainingDuration(0.0f)
    , m_isActive(false)
    , m_activeCaster(0, nullptr)
{
    m_abilityType = AbilityType::SelfBuff;
}

void SelfBuffAbility::Initialize() {
    Ability::Initialize();
    m_isActive = false;
    m_remainingDuration = 0.0f;
}

bool SelfBuffAbility::Use(Entity caster, Entity target) {
    if (!Ability::Use(caster)) {
        return false;
    }
    
    // If buff is already active, remove it first
    if (m_isActive && m_activeCaster.GetID() == caster.GetID()) {
        RemoveBuffEffects(m_activeCaster);
    }
    
    // Apply buff effects
    if (ApplyBuffEffects(caster)) {
        m_isActive = true;
        m_activeCaster = caster;
        m_remainingDuration = m_duration;
        return true;
    }
    
    return false;
}

void SelfBuffAbility::Update(float deltaTime) {
    Ability::Update(deltaTime);
    
    // Update buff duration
    if (m_isActive) {
        m_remainingDuration -= deltaTime;
        
        // Check if buff has expired
        if (m_remainingDuration <= 0.0f) {
            RemoveBuffEffects(m_activeCaster);
            m_isActive = false;
            m_activeCaster = Entity(0, nullptr);
            m_remainingDuration = 0.0f;
        }
    }
}

bool SelfBuffAbility::ApplyBuffEffects(Entity caster) {
    // Default implementation does nothing
    return true;
}

void SelfBuffAbility::RemoveBuffEffects(Entity caster) {
    // Default implementation does nothing
}

// DamageAbility implementation
DamageAbility::DamageAbility(const std::string& abilityId, const std::string& abilityName)
    : TargetedAbility(abilityId, abilityName)
{
}

void DamageAbility::Initialize() {
    TargetedAbility::Initialize();
}

bool DamageAbility::ApplyEffects(Entity caster, Entity target) {
    // Get hero components
    HeroComponent* casterHero = caster.GetComponent<HeroComponent>();
    HeroComponent* targetHero = target.GetComponent<HeroComponent>();
    
    if (!casterHero || !targetHero) {
        return false;
    }
    
    // Calculate damage
    float damage = GetDamage();
    
    // Add AP and AD scaling
    HeroStats casterStats = casterHero->GetCurrentStats();
    damage += casterStats.abilityPower * m_apRatio;
    damage += casterStats.attackDamage * m_adRatio;
    
    // Apply damage to target
    targetHero->TakeDamage(damage, m_isMagicalDamage);
    
    return true;
}

// HealAbility implementation
HealAbility::HealAbility(const std::string& abilityId, const std::string& abilityName)
    : TargetedAbility(abilityId, abilityName)
    , m_baseHealing(0.0f)
    , m_healingPerLevel(0.0f)
    , m_apRatio(0.0f)
{
}

void HealAbility::Initialize() {
    TargetedAbility::Initialize();
}

void HealAbility::SetHealing(float baseHealing, float healingPerLevel, float apRatio) {
    m_baseHealing = baseHealing;
    m_healingPerLevel = healingPerLevel;
    m_apRatio = apRatio;
}

float HealAbility::GetHealing() const {
    return m_baseHealing + (m_healingPerLevel * (m_level - 1));
}

bool HealAbility::ApplyEffects(Entity caster, Entity target) {
    // Get hero components
    HeroComponent* casterHero = caster.GetComponent<HeroComponent>();
    HeroComponent* targetHero = target.GetComponent<HeroComponent>();
    
    if (!casterHero || !targetHero) {
        return false;
    }
    
    // Calculate healing
    float healing = GetHealing();
    
    // Add AP scaling
    HeroStats casterStats = casterHero->GetCurrentStats();
    healing += casterStats.abilityPower * m_apRatio;
    
    // Apply healing to target
    targetHero->Heal(healing);
    
    return true;
}

// AreaDamageAbility implementation
AreaDamageAbility::AreaDamageAbility(const std::string& abilityId, const std::string& abilityName)
    : AreaOfEffectAbility(abilityId, abilityName)
{
}

void AreaDamageAbility::Initialize() {
    AreaOfEffectAbility::Initialize();
}

bool AreaDamageAbility::ApplyEffects(Entity caster, const glm::vec3& position, const std::vector<Entity>& targets) {
    // Get caster hero component
    HeroComponent* casterHero = caster.GetComponent<HeroComponent>();
    
    if (!casterHero) {
        return false;
    }
    
    // Calculate damage
    float damage = GetDamage();
    
    // Add AP and AD scaling
    HeroStats casterStats = casterHero->GetCurrentStats();
    damage += casterStats.abilityPower * m_apRatio;
    damage += casterStats.attackDamage * m_adRatio;
    
    // Apply damage to all targets
    for (Entity target : targets) {
        HeroComponent* targetHero = target.GetComponent<HeroComponent>();
        
        if (targetHero) {
            targetHero->TakeDamage(damage, m_isMagicalDamage);
        }
    }
    
    return true;
}

// SkillshotDamageAbility implementation
SkillshotDamageAbility::SkillshotDamageAbility(const std::string& abilityId, const std::string& abilityName)
    : SkillshotAbility(abilityId, abilityName)
{
}

void SkillshotDamageAbility::Initialize() {
    SkillshotAbility::Initialize();
}

bool SkillshotDamageAbility::ApplyEffects(Entity caster, const glm::vec3& origin, const glm::vec3& direction, const std::vector<Entity>& targets) {
    // Get caster hero component
    HeroComponent* casterHero = caster.GetComponent<HeroComponent>();
    
    if (!casterHero) {
        return false;
    }
    
    // Calculate damage
    float damage = GetDamage();
    
    // Add AP and AD scaling
    HeroStats casterStats = casterHero->GetCurrentStats();
    damage += casterStats.abilityPower * m_apRatio;
    damage += casterStats.attackDamage * m_adRatio;
    
    // Apply damage to all targets
    for (Entity target : targets) {
        HeroComponent* targetHero = target.GetComponent<HeroComponent>();
        
        if (targetHero) {
            targetHero->TakeDamage(damage, m_isMagicalDamage);
        }
    }
    
    return true;
}

// MovementSpeedBuffAbility implementation
MovementSpeedBuffAbility::MovementSpeedBuffAbility(const std::string& abilityId, const std::string& abilityName)
    : SelfBuffAbility(abilityId, abilityName)
    , m_movementSpeedBonus(0.0f)
{
}

void MovementSpeedBuffAbility::Initialize() {
    SelfBuffAbility::Initialize();
}

bool MovementSpeedBuffAbility::ApplyBuffEffects(Entity caster) {
    // Get hero component
    HeroComponent* heroComponent = caster.GetComponent<HeroComponent>();
    
    if (!heroComponent) {
        return false;
    }
    
    // Apply movement speed bonus
    // Note: This is a simplified implementation. In a real game, you would
    // have a more complex stat modification system.
    HeroStats currentStats = heroComponent->GetCurrentStats();
    currentStats.movementSpeed += m_movementSpeedBonus;
    
    // TODO: Apply modified stats to hero
    
    return true;
}

void MovementSpeedBuffAbility::RemoveBuffEffects(Entity caster) {
    // Get hero component
    HeroComponent* heroComponent = caster.GetComponent<HeroComponent>();
    
    if (!heroComponent) {
        return;
    }
    
    // Remove movement speed bonus
    // Note: This is a simplified implementation. In a real game, you would
    // have a more complex stat modification system.
    HeroStats currentStats = heroComponent->GetCurrentStats();
    currentStats.movementSpeed -= m_movementSpeedBonus;
    
    // TODO: Apply modified stats to hero
}

// AttackDamageBuffAbility implementation
AttackDamageBuffAbility::AttackDamageBuffAbility(const std::string& abilityId, const std::string& abilityName)
    : SelfBuffAbility(abilityId, abilityName)
    , m_attackDamageBonus(0.0f)
{
}

void AttackDamageBuffAbility::Initialize() {
    SelfBuffAbility::Initialize();
}

bool AttackDamageBuffAbility::ApplyBuffEffects(Entity caster) {
    // Get hero component
    HeroComponent* heroComponent = caster.GetComponent<HeroComponent>();
    
    if (!heroComponent) {
        return false;
    }
    
    // Apply attack damage bonus
    // Note: This is a simplified implementation. In a real game, you would
    // have a more complex stat modification system.
    HeroStats currentStats = heroComponent->GetCurrentStats();
    currentStats.attackDamage += m_attackDamageBonus;
    
    // TODO: Apply modified stats to hero
    
    return true;
}

void AttackDamageBuffAbility::RemoveBuffEffects(Entity caster) {
    // Get hero component
    HeroComponent* heroComponent = caster.GetComponent<HeroComponent>();
    
    if (!heroComponent) {
        return;
    }
    
    // Remove attack damage bonus
    // Note: This is a simplified implementation. In a real game, you would
    // have a more complex stat modification system.
    HeroStats currentStats = heroComponent->GetCurrentStats();
    currentStats.attackDamage -= m_attackDamageBonus;
    
    // TODO: Apply modified stats to hero
}

} // namespace CHULUBME
