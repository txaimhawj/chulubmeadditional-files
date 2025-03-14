# CHULUBME Hero Creation Workflow

This document provides a comprehensive guide for creating and testing heroes for the CHULUBME MOBA game.

## Overview

The CHULUBME hero system allows you to create, edit, and test heroes with various abilities. Heroes are the playable characters in the game, each with unique stats, abilities, and playstyles. This workflow guide will walk you through the process of creating and testing heroes using the provided tools.

## Prerequisites

Before starting, ensure you have:
- Compiled the CHULUBME game engine
- Set up the test environment
- Installed required dependencies (SDL2, OpenGL, ImGui)

## Hero Creation Workflow

### 1. Launch the Hero Test Environment

Run the test environment application:

```bash
./test_hero_environment
```

This will open the CHULUBME Hero Test Environment with the Hero Editor interface.

### 2. Create a New Hero

1. In the Hero Editor, click the "Add New Hero" button in the left panel.
2. Fill in the hero details in the popup:
   - **Hero ID**: A unique identifier for the hero (e.g., "frost_mage")
   - **Hero Name**: The display name of the hero (e.g., "Frost Mage")
   - **Description**: A brief description of the hero
   - **Role**: Select from Tank, Fighter, Assassin, Mage, Marksman, or Support
   - **Difficulty**: Set a difficulty rating from 1-10
3. Click "Create" to create the hero.

### 3. Configure Hero Stats

1. Select the newly created hero from the list in the left panel.
2. In the right panel, go to the "Stats" tab.
3. Configure the base stats:
   - **Health**: Base health points
   - **Mana**: Base mana points
   - **Attack Damage**: Base physical damage
   - **Ability Power**: Base magical damage
   - **Armor**: Physical damage reduction
   - **Magic Resist**: Magical damage reduction
   - **Attack Speed**: Attack frequency
   - **Movement Speed**: Movement velocity
   - **Health Regen**: Health regeneration per second
   - **Mana Regen**: Mana regeneration per second
   - **Crit Chance**: Probability of critical hits
   - **Crit Damage**: Multiplier for critical hits
   - **Life Steal**: Percentage of damage converted to health
   - **Cooldown Reduction**: Reduction in ability cooldowns
4. Configure the per-level stat growth:
   - **Health Per Level**: Health gained per level
   - **Mana Per Level**: Mana gained per level
   - **Attack Damage Per Level**: Attack damage gained per level
   - **Ability Power Per Level**: Ability power gained per level
   - **Armor Per Level**: Armor gained per level
   - **Magic Resist Per Level**: Magic resist gained per level
   - **Attack Speed Per Level**: Attack speed gained per level

### 4. Create Hero Abilities

1. Go to the "Abilities" tab in the right panel.
2. Click "Add New Ability" to create a new ability.
3. Fill in the ability details in the popup:
   - **Ability ID**: A unique identifier for the ability
   - **Ability Name**: The display name of the ability
   - **Description**: A description of what the ability does
   - **Type**: Select from Targeted, Area of Effect, Skillshot, or Self Buff
   - **Cooldown**: Time in seconds before the ability can be used again
   - **Mana Cost**: Mana required to use the ability
   - **Range**: Distance at which the ability can be used
4. Click "Create" to proceed to the ability type selection.
5. Select the specific type of ability:
   - **Damage Ability**: Deals damage to a single target
   - **Heal Ability**: Heals a target
   - **Area Damage Ability**: Deals damage in an area
   - **Skillshot Damage Ability**: Deals damage in a line
   - **Movement Speed Buff**: Increases movement speed
   - **Attack Damage Buff**: Increases attack damage

### 5. Configure Ability Parameters

Depending on the ability type selected, configure the specific parameters:

#### For Damage Abilities:
- **Base Damage**: Initial damage amount
- **Damage Per Level**: Additional damage per ability level
- **AP Ratio**: Scaling with ability power
- **AD Ratio**: Scaling with attack damage
- **Magical Damage**: Whether the damage is magical or physical

#### For Heal Abilities:
- **Base Healing**: Initial healing amount
- **Healing Per Level**: Additional healing per ability level
- **AP Ratio**: Scaling with ability power

#### For Area Abilities:
- **Radius**: Size of the area effect
- Damage parameters (same as Damage Abilities)

#### For Skillshot Abilities:
- **Width**: Width of the skillshot
- **Speed**: Travel speed of the skillshot
- Damage parameters (same as Damage Abilities)

#### For Buff Abilities:
- **Duration**: How long the buff lasts
- **Movement Speed Bonus**: For movement speed buffs
- **Attack Damage Bonus**: For attack damage buffs

### 6. Test the Hero

1. In the "Hero Test" window, select your hero from the list.
2. View the hero's stats and abilities.
3. Use the test buttons to:
   - **Level Up**: Increase the hero's level
   - **Take Damage**: Test damage taking
   - **Heal**: Test healing

### 7. Save and Load Heroes

1. To save heroes, click "Save Heroes" in the left panel or use File > Save Heroes.
2. To load heroes, click "Load Heroes" in the left panel or use File > Load Heroes.
3. Heroes are saved in JSON format in the "heroes.json" file.

## Sample Heroes

The system comes with sample heroes that you can load and examine:

1. **Axe Master**: A powerful warrior specializing in close combat
   - **Abilities**: Cleaving Strike, Battle Cry, Axe Throw, Berserker Rage

2. **Frost Mage**: An ice mage who controls the battlefield with freezing spells
   - **Abilities**: Ice Bolt, Frost Nova, Ice Barrier, Blizzard

3. **Shadow Assassin**: A deadly assassin who strikes from the shadows
   - **Abilities**: Shadow Strike, Shadow Step, Throwing Daggers, Death Mark

To load these sample heroes, copy the `sample_heroes.json` file to `heroes.json` and click "Load Heroes" in the Hero Editor.

## Hero JSON Format

Heroes are stored in JSON format with the following structure:

```json
{
  "heroes": [
    {
      "id": "hero_id",
      "name": "Hero Name",
      "description": "Hero description",
      "role": 0,
      "difficulty": 5,
      "stats": {
        "health": 600.0,
        "mana": 300.0,
        // Other stats...
      },
      "abilities": [
        {
          "id": "ability_id",
          "name": "Ability Name",
          "description": "Ability description",
          "type": "AbilityType",
          "cooldown": 10.0,
          "manaCost": 50.0,
          "range": 500.0,
          "level": 1,
          // Type-specific parameters...
        }
        // More abilities...
      ]
    }
    // More heroes...
  ]
}
```

## Advanced Usage

### Creating Custom Ability Types

To create custom ability types:

1. Extend one of the base ability classes (TargetedAbility, AreaOfEffectAbility, SkillshotAbility, SelfBuffAbility)
2. Override the necessary methods (Initialize, ApplyEffects, etc.)
3. Register the new ability type with the AbilitySystem

Example:
```cpp
class MyCustomAbility : public TargetedAbility {
public:
    MyCustomAbility(const std::string& abilityId = "", const std::string& abilityName = "")
        : TargetedAbility(abilityId, abilityName) {
    }
    
    void Initialize() override {
        TargetedAbility::Initialize();
        // Custom initialization
    }
    
    bool ApplyEffects(Entity caster, Entity target) override {
        // Custom effect application
        return true;
    }
};

// Register with ability system
abilitySystem->RegisterAbilityType<MyCustomAbility>("MyCustomAbility");
```

### Integrating with Blockchain for NFT Skins

The hero system is designed to integrate with the CHULUBME blockchain for NFT skins:

1. Each hero has a `SetSkin` method that accepts an NFT skin ID
2. The blockchain interface provides methods to check NFT ownership
3. When a skin is applied, the hero's visual appearance changes

To implement this integration:
1. Use the BlockchainInterface to query owned NFTs
2. Apply the NFT skin to the hero using the SetSkin method
3. The rendering system will automatically use the appropriate model and textures

## Troubleshooting

### Common Issues

1. **Hero not appearing in the list**:
   - Ensure the hero was created successfully
   - Check for errors in the console output

2. **Abilities not working**:
   - Verify that the ability parameters are set correctly
   - Check that the hero has enough mana to use the ability

3. **Changes not saving**:
   - Ensure you have write permissions for the heroes.json file
   - Check for disk space issues

### Getting Help

For additional help:
1. Check the engine documentation
2. Review the source code comments
3. Contact the CHULUBME development team

## Conclusion

This workflow guide provides the essential steps for creating and testing heroes in the CHULUBME MOBA game. By following these steps, you can create unique heroes with diverse abilities and playstyles, enhancing the gameplay experience for players.

Remember that hero balance is crucial for a competitive MOBA game. Test your heroes thoroughly to ensure they are balanced and fun to play.
