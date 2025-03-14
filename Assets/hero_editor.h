#pragma once

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <imgui.h>

#include "../core/ecs.h"
#include "hero_system.h"
#include "ability_types.h"

namespace CHULUBME {

/**
 * @brief Hero Editor class for creating and editing heroes
 */
class HeroEditor {
public:
    // Singleton instance
    static HeroEditor& Instance();

    // Initialize the hero editor
    bool Initialize();

    // Shutdown the hero editor
    void Shutdown();

    // Render the hero editor UI
    void Render();

    // Set the entity manager
    void SetEntityManager(EntityManager* entityManager) { m_entityManager = entityManager; }

    // Set the hero system
    void SetHeroSystem(HeroSystem* heroSystem) { m_heroSystem = heroSystem; }

    // Set the ability system
    void SetAbilitySystem(AbilitySystem* abilitySystem) { m_abilitySystem = abilitySystem; }

    // Create a new hero
    Entity CreateHero(const std::string& heroId, const std::string& heroName);

    // Load heroes from file
    bool LoadHeroes(const std::string& filename);

    // Save heroes to file
    bool SaveHeroes(const std::string& filename);

private:
    // Private constructor for singleton
    HeroEditor();
    ~HeroEditor();

    // Deleted copy constructor and assignment operator
    HeroEditor(const HeroEditor&) = delete;
    HeroEditor& operator=(const HeroEditor&) = delete;

    // Render hero list
    void RenderHeroList();

    // Render hero details
    void RenderHeroDetails();

    // Render hero stats
    void RenderHeroStats();

    // Render hero abilities
    void RenderHeroAbilities();

    // Render ability editor
    void RenderAbilityEditor();

    // Render ability type selector
    void RenderAbilityTypeSelector();

    // Entity manager reference
    EntityManager* m_entityManager;

    // Hero system reference
    HeroSystem* m_heroSystem;

    // Ability system reference
    AbilitySystem* m_abilitySystem;

    // Currently selected hero
    Entity m_selectedHero;

    // Currently selected ability
    std::shared_ptr<Ability> m_selectedAbility;

    // New hero data
    struct NewHeroData {
        char id[64] = "";
        char name[64] = "";
        char description[256] = "";
        int role = 0;
        int difficulty = 5;
    };
    NewHeroData m_newHeroData;

    // New ability data
    struct NewAbilityData {
        char id[64] = "";
        char name[64] = "";
        char description[256] = "";
        int type = 0;
        float cooldown = 10.0f;
        float manaCost = 50.0f;
        float range = 500.0f;
    };
    NewAbilityData m_newAbilityData;

    // Hero stat editor data
    struct HeroStatData {
        float health = 600.0f;
        float mana = 300.0f;
        float attackDamage = 60.0f;
        float abilityPower = 0.0f;
        float armor = 30.0f;
        float magicResist = 30.0f;
        float attackSpeed = 0.7f;
        float movementSpeed = 350.0f;
        float healthRegen = 5.0f;
        float manaRegen = 3.0f;
        float critChance = 0.0f;
        float critDamage = 2.0f;
        float lifeSteal = 0.0f;
        float cooldownReduction = 0.0f;
        float healthPerLevel = 90.0f;
        float manaPerLevel = 40.0f;
        float attackDamagePerLevel = 3.0f;
        float abilityPowerPerLevel = 0.0f;
        float armorPerLevel = 3.5f;
        float magicResistPerLevel = 1.25f;
        float attackSpeedPerLevel = 0.02f;
    };
    HeroStatData m_heroStatData;

    // Ability editor data
    struct AbilityEditorData {
        // Common ability data
        char id[64] = "";
        char name[64] = "";
        char description[256] = "";
        int type = 0;
        float cooldown = 10.0f;
        float manaCost = 50.0f;
        float range = 500.0f;

        // Damage ability data
        float baseDamage = 100.0f;
        float damagePerLevel = 50.0f;
        float apRatio = 0.6f;
        float adRatio = 0.0f;
        bool isMagicalDamage = true;

        // Heal ability data
        float baseHealing = 100.0f;
        float healingPerLevel = 50.0f;
        float healingApRatio = 0.7f;

        // Area ability data
        float radius = 300.0f;

        // Skillshot ability data
        float width = 100.0f;
        float speed = 1500.0f;

        // Buff ability data
        float duration = 5.0f;
        float movementSpeedBonus = 50.0f;
        float attackDamageBonus = 30.0f;
    };
    AbilityEditorData m_abilityEditorData;

    // UI state
    bool m_showNewHeroPopup = false;
    bool m_showNewAbilityPopup = false;
    bool m_showAbilityTypeSelector = false;
    int m_selectedAbilityType = 0;
};

} // namespace CHULUBME
