#include "hero_editor.h"
#include <fstream>
#include <json/json.h>

namespace CHULUBME {

// Singleton implementation
HeroEditor& HeroEditor::Instance() {
    static HeroEditor instance;
    return instance;
}

HeroEditor::HeroEditor()
    : m_entityManager(nullptr)
    , m_heroSystem(nullptr)
    , m_abilitySystem(nullptr)
    , m_selectedHero(0, nullptr)
    , m_selectedAbility(nullptr)
    , m_showNewHeroPopup(false)
    , m_showNewAbilityPopup(false)
    , m_showAbilityTypeSelector(false)
    , m_selectedAbilityType(0)
{
}

HeroEditor::~HeroEditor() {
    Shutdown();
}

bool HeroEditor::Initialize() {
    // Reset editor state
    m_selectedHero = Entity(0, nullptr);
    m_selectedAbility = nullptr;
    m_showNewHeroPopup = false;
    m_showNewAbilityPopup = false;
    m_showAbilityTypeSelector = false;
    m_selectedAbilityType = 0;
    
    return true;
}

void HeroEditor::Shutdown() {
    // Nothing to do here
}

void HeroEditor::Render() {
    if (!m_entityManager || !m_heroSystem || !m_abilitySystem) {
        ImGui::Text("Hero Editor not properly initialized!");
        return;
    }
    
    // Main editor window
    ImGui::Begin("Hero Editor");
    
    // Split view: hero list on left, details on right
    ImGui::Columns(2, "heroEditorColumns");
    
    // Left column: hero list
    RenderHeroList();
    
    ImGui::NextColumn();
    
    // Right column: hero details
    RenderHeroDetails();
    
    ImGui::Columns(1);
    
    ImGui::End();
    
    // Render popups
    if (m_showNewHeroPopup) {
        ImGui::OpenPopup("Create New Hero");
        ImGui::SetNextWindowSize(ImVec2(400, 300));
        if (ImGui::BeginPopupModal("Create New Hero", &m_showNewHeroPopup)) {
            ImGui::InputText("Hero ID", m_newHeroData.id, sizeof(m_newHeroData.id));
            ImGui::InputText("Hero Name", m_newHeroData.name, sizeof(m_newHeroData.name));
            ImGui::InputTextMultiline("Description", m_newHeroData.description, sizeof(m_newHeroData.description));
            
            const char* roles[] = { "Tank", "Fighter", "Assassin", "Mage", "Marksman", "Support" };
            ImGui::Combo("Role", &m_newHeroData.role, roles, IM_ARRAYSIZE(roles));
            
            ImGui::SliderInt("Difficulty", &m_newHeroData.difficulty, 1, 10);
            
            if (ImGui::Button("Create")) {
                // Create new hero
                Entity hero = CreateHero(m_newHeroData.id, m_newHeroData.name);
                
                if (hero.IsActive()) {
                    HeroComponent* heroComponent = hero.GetComponent<HeroComponent>();
                    if (heroComponent) {
                        heroComponent->SetDescription(m_newHeroData.description);
                        heroComponent->SetRole(static_cast<HeroComponent::HeroRole>(m_newHeroData.role));
                        heroComponent->SetDifficulty(m_newHeroData.difficulty);
                    }
                    
                    // Select the new hero
                    m_selectedHero = hero;
                    
                    // Reset new hero data
                    memset(&m_newHeroData, 0, sizeof(m_newHeroData));
                    m_newHeroData.difficulty = 5;
                    
                    m_showNewHeroPopup = false;
                }
            }
            
            ImGui::SameLine();
            
            if (ImGui::Button("Cancel")) {
                m_showNewHeroPopup = false;
            }
            
            ImGui::EndPopup();
        }
    }
    
    if (m_showNewAbilityPopup) {
        ImGui::OpenPopup("Create New Ability");
        ImGui::SetNextWindowSize(ImVec2(400, 300));
        if (ImGui::BeginPopupModal("Create New Ability", &m_showNewAbilityPopup)) {
            ImGui::InputText("Ability ID", m_newAbilityData.id, sizeof(m_newAbilityData.id));
            ImGui::InputText("Ability Name", m_newAbilityData.name, sizeof(m_newAbilityData.name));
            ImGui::InputTextMultiline("Description", m_newAbilityData.description, sizeof(m_newAbilityData.description));
            
            const char* types[] = { "Targeted", "Area of Effect", "Skillshot", "Self Buff" };
            ImGui::Combo("Type", &m_newAbilityData.type, types, IM_ARRAYSIZE(types));
            
            ImGui::InputFloat("Cooldown", &m_newAbilityData.cooldown);
            ImGui::InputFloat("Mana Cost", &m_newAbilityData.manaCost);
            ImGui::InputFloat("Range", &m_newAbilityData.range);
            
            if (ImGui::Button("Create")) {
                if (m_selectedHero.IsActive()) {
                    HeroComponent* heroComponent = m_selectedHero.GetComponent<HeroComponent>();
                    if (heroComponent) {
                        // Show ability type selector
                        m_showAbilityTypeSelector = true;
                        m_showNewAbilityPopup = false;
                    }
                }
            }
            
            ImGui::SameLine();
            
            if (ImGui::Button("Cancel")) {
                m_showNewAbilityPopup = false;
            }
            
            ImGui::EndPopup();
        }
    }
    
    if (m_showAbilityTypeSelector) {
        RenderAbilityTypeSelector();
    }
}

void HeroEditor::RenderHeroList() {
    ImGui::Text("Heroes");
    ImGui::Separator();
    
    // Add new hero button
    if (ImGui::Button("Add New Hero")) {
        m_showNewHeroPopup = true;
    }
    
    // Save/load buttons
    ImGui::SameLine();
    if (ImGui::Button("Save Heroes")) {
        SaveHeroes("heroes.json");
    }
    
    ImGui::SameLine();
    if (ImGui::Button("Load Heroes")) {
        LoadHeroes("heroes.json");
    }
    
    // Hero list
    ImGui::BeginChild("HeroList", ImVec2(0, 0), true);
    
    std::vector<Entity> heroes = m_heroSystem->GetAllHeroes();
    for (Entity hero : heroes) {
        HeroComponent* heroComponent = hero.GetComponent<HeroComponent>();
        if (heroComponent) {
            std::string label = heroComponent->GetHeroName() + " (" + heroComponent->GetHeroID() + ")";
            bool isSelected = (m_selectedHero.IsActive() && m_selectedHero.GetID() == hero.GetID());
            
            if (ImGui::Selectable(label.c_str(), isSelected)) {
                m_selectedHero = hero;
                m_selectedAbility = nullptr;
            }
        }
    }
    
    ImGui::EndChild();
}

void HeroEditor::RenderHeroDetails() {
    if (!m_selectedHero.IsActive()) {
        ImGui::Text("No hero selected");
        return;
    }
    
    HeroComponent* heroComponent = m_selectedHero.GetComponent<HeroComponent>();
    if (!heroComponent) {
        ImGui::Text("Selected entity is not a hero");
        return;
    }
    
    // Hero details
    ImGui::Text("Hero Details");
    ImGui::Separator();
    
    // Hero ID and name
    char heroId[64];
    strncpy(heroId, heroComponent->GetHeroID().c_str(), sizeof(heroId));
    if (ImGui::InputText("Hero ID", heroId, sizeof(heroId))) {
        heroComponent->SetHeroID(heroId);
    }
    
    char heroName[64];
    strncpy(heroName, heroComponent->GetHeroName().c_str(), sizeof(heroName));
    if (ImGui::InputText("Hero Name", heroName, sizeof(heroName))) {
        heroComponent->SetHeroName(heroName);
    }
    
    // Hero description
    char description[256];
    strncpy(description, heroComponent->GetDescription().c_str(), sizeof(description));
    if (ImGui::InputTextMultiline("Description", description, sizeof(description))) {
        heroComponent->SetDescription(description);
    }
    
    // Hero role
    const char* roles[] = { "Tank", "Fighter", "Assassin", "Mage", "Marksman", "Support" };
    int role = static_cast<int>(heroComponent->GetRole());
    if (ImGui::Combo("Role", &role, roles, IM_ARRAYSIZE(roles))) {
        heroComponent->SetRole(static_cast<HeroComponent::HeroRole>(role));
    }
    
    // Hero difficulty
    int difficulty = heroComponent->GetDifficulty();
    if (ImGui::SliderInt("Difficulty", &difficulty, 1, 10)) {
        heroComponent->SetDifficulty(difficulty);
    }
    
    // Tabs for stats and abilities
    if (ImGui::BeginTabBar("HeroDetailsTabs")) {
        if (ImGui::BeginTabItem("Stats")) {
            RenderHeroStats();
            ImGui::EndTabItem();
        }
        
        if (ImGui::BeginTabItem("Abilities")) {
            RenderHeroAbilities();
            ImGui::EndTabItem();
        }
        
        ImGui::EndTabBar();
    }
}

void HeroEditor::RenderHeroStats() {
    if (!m_selectedHero.IsActive()) {
        return;
    }
    
    HeroComponent* heroComponent = m_selectedHero.GetComponent<HeroComponent>();
    if (!heroComponent) {
        return;
    }
    
    // Get current stats
    HeroStats stats = heroComponent->GetBaseStats();
    
    // Copy stats to editor data if not already done
    if (m_heroStatData.health != stats.health) {
        m_heroStatData.health = stats.health;
        m_heroStatData.mana = stats.mana;
        m_heroStatData.attackDamage = stats.attackDamage;
        m_heroStatData.abilityPower = stats.abilityPower;
        m_heroStatData.armor = stats.armor;
        m_heroStatData.magicResist = stats.magicResist;
        m_heroStatData.attackSpeed = stats.attackSpeed;
        m_heroStatData.movementSpeed = stats.movementSpeed;
        m_heroStatData.healthRegen = stats.healthRegen;
        m_heroStatData.manaRegen = stats.manaRegen;
        m_heroStatData.critChance = stats.critChance;
        m_heroStatData.critDamage = stats.critDamage;
        m_heroStatData.lifeSteal = stats.lifeSteal;
        m_heroStatData.cooldownReduction = stats.cooldownReduction;
        m_heroStatData.healthPerLevel = stats.healthPerLevel;
        m_heroStatData.manaPerLevel = stats.manaPerLevel;
        m_heroStatData.attackDamagePerLevel = stats.attackDamagePerLevel;
        m_heroStatData.abilityPowerPerLevel = stats.abilityPowerPerLevel;
        m_heroStatData.armorPerLevel = stats.armorPerLevel;
        m_heroStatData.magicResistPerLevel = stats.magicResistPerLevel;
        m_heroStatData.attackSpeedPerLevel = stats.attackSpeedPerLevel;
    }
    
    // Base stats
    ImGui::Text("Base Stats");
    ImGui::Separator();
    
    bool statsChanged = false;
    
    statsChanged |= ImGui::InputFloat("Health", &m_heroStatData.health);
    statsChanged |= ImGui::InputFloat("Mana", &m_heroStatData.mana);
    statsChanged |= ImGui::InputFloat("Attack Damage", &m_heroStatData.attackDamage);
    statsChanged |= ImGui::InputFloat("Ability Power", &m_heroStatData.abilityPower);
    statsChanged |= ImGui::InputFloat("Armor", &m_heroStatData.armor);
    statsChanged |= ImGui::InputFloat("Magic Resist", &m_heroStatData.magicResist);
    statsChanged |= ImGui::InputFloat("Attack Speed", &m_heroStatData.attackSpeed);
    statsChanged |= ImGui::InputFloat("Movement Speed", &m_heroStatData.movementSpeed);
    statsChanged |= ImGui::InputFloat("Health Regen", &m_heroStatData.healthRegen);
    statsChanged |= ImGui::InputFloat("Mana Regen", &m_heroStatData.manaRegen);
    statsChanged |= ImGui::InputFloat("Crit Chance", &m_heroStatData.critChance);
    statsChanged |= ImGui::InputFloat("Crit Damage", &m_heroStatData.critDamage);
    statsChanged |= ImGui::InputFloat("Life Steal", &m_heroStatData.lifeSteal);
    statsChanged |= ImGui::InputFloat("Cooldown Reduction", &m_heroStatData.cooldownReduction);
    
    // Per level stats
    ImGui::Text("Per Level Stats");
    ImGui::Separator();
    
    statsChanged |= ImGui::InputFloat("Health Per Level", &m_heroStatData.healthPerLevel);
    statsChanged |= ImGui::InputFloat("Mana Per Level", &m_heroStatData.manaPerLevel);
    statsChanged |= ImGui::InputFloat("Attack Damage Per Level", &m_heroStatData.attackDamagePerLevel);
    statsChanged |= ImGui::InputFloat("Ability Power Per Level", &m_heroStatData.abilityPowerPerLevel);
    statsChanged |= ImGui::InputFloat("Armor Per Level", &m_heroStatData.armorPerLevel);
    statsChanged |= ImGui::InputFloat("Magic Resist Per Level", &m_heroStatData.magicResistPerLevel);
    statsChanged |= ImGui::InputFloat("Attack Speed Per Level", &m_heroStatData.attackSpeedPerLevel);
    
    // Apply changes
    if (statsChanged) {
        HeroStats newStats;
        newStats.health = m_heroStatData.health;
        newStats.mana = m_heroStatData.mana;
        newStats.attackDamage = m_heroStatData.attackDamage;
        newStats.abilityPower = m_heroStatData.abilityPower;
        newStats.armor = m_heroStatData.armor;
        newStats.magicResist = m_heroStatData.magicResist;
        newStats.attackSpeed = m_heroStatData.attackSpeed;
        newStats.movementSpeed = m_heroStatData.movementSpeed;
        newStats.healthRegen = m_heroStatData.healthRegen;
        newStats.manaRegen = m_heroStatData.manaRegen;
        newStats.critChance = m_heroStatData.critChance;
        newStats.critDamage = m_heroStatData.critDamage;
        newStats.lifeSteal = m_heroStatData.lifeSteal;
        newStats.cooldownReduction = m_heroStatData.cooldownReduction;
        newStats.healthPerLevel = m_heroStatData.healthPerLevel;
        newStats.manaPerLevel = m_heroStatData.manaPerLevel;
        newStats.attackDamagePerLevel = m_heroStatData.attackDamagePerLevel;
        newStats.abilityPowerPerLevel = m_heroStatData.abilityPowerPerLevel;
        newStats.armorPerLevel = m_heroStatData.armorPerLevel;
        newStats.magicResistPerLevel = m_heroStatData.magicResistPerLevel;
        newStats.attackSpeedPerLevel = m_heroStatData.attackSpeedPerLevel;
        
        heroComponent->SetBaseStats(newStats);
    }
}

void HeroEditor::RenderHeroAbilities() {
    if (!m_selectedHero.IsActive()) {
        return;
    }
    
    HeroComponent* heroComponent = m_selectedHero.GetComponent<HeroComponent>();
    if (!heroComponent) {
        return;
    }
    
    // Add new ability button
    if (ImGui::Button("Add New Ability")) {
        m_showNewAbilityPopup = true;
    }
    
    // Ability list
    ImGui::BeginChild("AbilityList", ImVec2(0, 150), true);
    
    const std::vector<std::shared_ptr<Ability>>& abilities = heroComponent->GetAbilities();
    for (size_t i = 0; i < abilities.size(); ++i) {
        std::shared_ptr<Ability> ability = abilities[i];
        std::string label = ability->GetAbilityName() + " (" + ability->GetAbilityID() + ")";
        bool isSelected = (m_selectedAbility == ability);
        
        if (ImGui::Selectable(label.c_str(), isSelected)) {
            m_selectedAbility = ability;
        }
    }
    
    ImGui::EndChild();
    
    // Ability editor
    if (m_selectedAbility) {
        RenderAbilityEditor();
    }
}

void HeroEditor::RenderAbilityEditor() {
    if (!m_selectedAbility) {
        return;
    }
    
    ImGui::Text("Ability Editor");
    ImGui::Separator();
    
    // Ability ID and name
    char abilityId[64];
    strncpy(abilityId, m_selectedAbility->GetAbilityID().c_str(), sizeof(abilityId));
    if (ImGui::InputText("Ability ID", abilityId, sizeof(abilityId))) {
        m_selectedAbility->SetAbilityID(abilityId);
    }
    
    char abilityName[64];
    strncpy(abilityName, m_selectedAbility->GetAbilityName().c_str(), sizeof(abilityName));
    if (ImGui::InputText("Ability Name", abilityName, sizeof(abilityName))) {
        m_selectedAbility->SetAbilityName(abilityName);
    }
    
    // Ability description
    char description[256];
    strncpy(description, m_selectedAbility->GetDescription().c_str(), sizeof(description));
    if (ImGui::InputTextMultiline("Description", description, sizeof(description))) {
        m_selectedAbility->SetDescription(description);
    }
    
    // Ability type
    const char* types[] = { "Passive", "Targeted", "Area of Effect", "Skillshot", "Self Buff" };
    int type = static_cast<int>(m_selectedAbility->GetAbilityType());
    ImGui::LabelText("Type", "%s", types[type]);
    
    // Common ability parameters
    float cooldown = m_selectedAbility->GetCooldown();
    i<response clipped><NOTE>To save on context only part of this file has been shown to you. You should retry this tool after you have searched inside the file with `grep -n` in order to find the line numbers of what you are looking for.</NOTE>