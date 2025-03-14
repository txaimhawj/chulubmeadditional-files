#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <SDL2/SDL.h>
#include <imgui.h>
#include <imgui_impl_sdl.h>
#include <imgui_impl_opengl3.h>
#include <GL/glew.h>

#include "core/ecs.h"
#include "core/engine.h"
#include "core/memory.h"
#include "rendering/renderer.h"
#include "input/input_manager.h"
#include "gameplay/hero_system.h"
#include "gameplay/ability_types.h"
#include "gameplay/hero_editor.h"

using namespace CHULUBME;

// Test environment for heroes
class HeroTestEnvironment {
public:
    HeroTestEnvironment() 
        : m_window(nullptr)
        , m_glContext(nullptr)
        , m_running(false)
    {
    }

    ~HeroTestEnvironment() {
        Shutdown();
    }

    bool Initialize() {
        // Initialize SDL
        if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
            std::cerr << "Error initializing SDL: " << SDL_GetError() << std::endl;
            return false;
        }

        // Set OpenGL attributes
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
        SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

        // Create window
        m_window = SDL_CreateWindow(
            "CHULUBME Hero Test Environment",
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            1280,
            720,
            SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI
        );

        if (!m_window) {
            std::cerr << "Error creating SDL window: " << SDL_GetError() << std::endl;
            return false;
        }

        // Create OpenGL context
        m_glContext = SDL_GL_CreateContext(m_window);
        if (!m_glContext) {
            std::cerr << "Error creating OpenGL context: " << SDL_GetError() << std::endl;
            return false;
        }

        SDL_GL_MakeCurrent(m_window, m_glContext);
        SDL_GL_SetSwapInterval(1); // Enable vsync

        // Initialize GLEW
        GLenum glewError = glewInit();
        if (glewError != GLEW_OK) {
            std::cerr << "Error initializing GLEW: " << glewGetErrorString(glewError) << std::endl;
            return false;
        }

        // Initialize ImGui
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

        ImGui::StyleColorsDark();

        ImGui_ImplSDL2_InitForOpenGL(m_window, m_glContext);
        ImGui_ImplOpenGL3_Init("#version 330");

        // Initialize engine
        Engine& engine = Engine::Instance();
        if (!engine.Initialize()) {
            std::cerr << "Failed to initialize engine" << std::endl;
            return false;
        }

        // Initialize input manager
        InputManager& inputManager = InputManager::Instance();
        if (!inputManager.Initialize()) {
            std::cerr << "Failed to initialize input manager" << std::endl;
            return false;
        }

        // Initialize memory manager
        MemoryManager& memoryManager = MemoryManager::Instance();
        if (!memoryManager.Initialize()) {
            std::cerr << "Failed to initialize memory manager" << std::endl;
            return false;
        }

        // Get entity manager
        m_entityManager = engine.GetEntityManager();

        // Register hero system
        m_heroSystem = m_entityManager->RegisterSystem<HeroSystem>();

        // Register ability system
        m_abilitySystem = m_entityManager->RegisterSystem<AbilitySystem>();

        // Register ability types
        m_abilitySystem->RegisterAbilityType<DamageAbility>("DamageAbility");
        m_abilitySystem->RegisterAbilityType<HealAbility>("HealAbility");
        m_abilitySystem->RegisterAbilityType<AreaDamageAbility>("AreaDamageAbility");
        m_abilitySystem->RegisterAbilityType<SkillshotDamageAbility>("SkillshotDamageAbility");
        m_abilitySystem->RegisterAbilityType<MovementSpeedBuffAbility>("MovementSpeedBuffAbility");
        m_abilitySystem->RegisterAbilityType<AttackDamageBuffAbility>("AttackDamageBuffAbility");

        // Initialize hero editor
        HeroEditor& heroEditor = HeroEditor::Instance();
        heroEditor.SetEntityManager(m_entityManager);
        heroEditor.SetHeroSystem(m_heroSystem);
        heroEditor.SetAbilitySystem(m_abilitySystem);
        if (!heroEditor.Initialize()) {
            std::cerr << "Failed to initialize hero editor" << std::endl;
            return false;
        }

        m_running = true;
        return true;
    }

    void Run() {
        while (m_running) {
            // Handle events
            SDL_Event event;
            while (SDL_PollEvent(&event)) {
                ImGui_ImplSDL2_ProcessEvent(&event);
                
                if (event.type == SDL_QUIT) {
                    m_running = false;
                }
                
                if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && 
                    event.window.windowID == SDL_GetWindowID(m_window)) {
                    m_running = false;
                }
            }

            // Start ImGui frame
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplSDL2_NewFrame(m_window);
            ImGui::NewFrame();

            // Render hero editor
            HeroEditor::Instance().Render();

            // Render test environment UI
            RenderUI();

            // Render ImGui
            ImGui::Render();
            
            // Clear screen
            glViewport(0, 0, (int)ImGui::GetIO().DisplaySize.x, (int)ImGui::GetIO().DisplaySize.y);
            glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);
            
            // Render ImGui
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            // Swap buffers
            SDL_GL_SwapWindow(m_window);
        }
    }

    void Shutdown() {
        // Shutdown hero editor
        HeroEditor::Instance().Shutdown();

        // Shutdown engine
        Engine::Instance().Shutdown();

        // Shutdown ImGui
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplSDL2_Shutdown();
        ImGui::DestroyContext();

        // Shutdown SDL
        if (m_glContext) {
            SDL_GL_DeleteContext(m_glContext);
            m_glContext = nullptr;
        }

        if (m_window) {
            SDL_DestroyWindow(m_window);
            m_window = nullptr;
        }

        SDL_Quit();
    }

private:
    void RenderUI() {
        // Main menu bar
        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("File")) {
                if (ImGui::MenuItem("New Hero")) {
                    // Create a new hero
                    Entity hero = m_heroSystem->CreateHero("hero_" + std::to_string(rand() % 1000), "New Hero");
                }
                
                if (ImGui::MenuItem("Save Heroes")) {
                    // Save heroes
                    m_heroSystem->SaveHeroData("heroes.json");
                }
                
                if (ImGui::MenuItem("Load Heroes")) {
                    // Load heroes
                    m_heroSystem->LoadHeroData("heroes.json");
                }
                
                ImGui::Separator();
                
                if (ImGui::MenuItem("Exit")) {
                    m_running = false;
                }
                
                ImGui::EndMenu();
            }
            
            if (ImGui::BeginMenu("View")) {
                if (ImGui::MenuItem("Hero Editor")) {
                    // Hero editor is always visible
                }
                
                ImGui::EndMenu();
            }
            
            if (ImGui::BeginMenu("Help")) {
                if (ImGui::MenuItem("About")) {
                    m_showAboutWindow = true;
                }
                
                ImGui::EndMenu();
            }
            
            ImGui::EndMainMenuBar();
        }

        // About window
        if (m_showAboutWindow) {
            ImGui::Begin("About CHULUBME Hero Test Environment", &m_showAboutWindow);
            ImGui::Text("CHULUBME Hero Test Environment");
            ImGui::Separator();
            ImGui::Text("This is a test environment for creating and testing heroes for the CHULUBME MOBA game.");
            ImGui::Text("Use the Hero Editor to create and edit heroes and their abilities.");
            ImGui::End();
        }

        // Hero test window
        ImGui::Begin("Hero Test");
        
        // Get all heroes
        std::vector<Entity> heroes = m_heroSystem->GetAllHeroes();
        
        // Display hero count
        ImGui::Text("Heroes: %zu", heroes.size());
        
        // Display hero list
        if (ImGui::BeginListBox("##HeroList", ImVec2(-1, 200))) {
            for (Entity hero : heroes) {
                HeroComponent* heroComponent = hero.GetComponent<HeroComponent>();
                if (heroComponent) {
                    std::string label = heroComponent->GetHeroName() + " (" + heroComponent->GetHeroID() + ")";
                    bool isSelected = (m_selectedTestHero.IsActive() && m_selectedTestHero.GetID() == hero.GetID());
                    
                    if (ImGui::Selectable(label.c_str(), isSelected)) {
                        m_selectedTestHero = hero;
                    }
                }
            }
            ImGui::EndListBox();
        }
        
        // Display selected hero details
        if (m_selectedTestHero.IsActive()) {
            HeroComponent* heroComponent = m_selectedTestHero.GetComponent<HeroComponent>();
            if (heroComponent) {
                ImGui::Separator();
                ImGui::Text("Selected Hero: %s", heroComponent->GetHeroName().c_str());
                
                // Display hero stats
                HeroStats stats = heroComponent->GetCurrentStats();
                ImGui::Text("Health: %.1f", stats.health);
                ImGui::Text("Mana: %.1f", stats.mana);
                ImGui::Text("Attack Damage: %.1f", stats.attackDamage);
                ImGui::Text("Ability Power: %.1f", stats.abilityPower);
                ImGui::Text("Armor: %.1f", stats.armor);
                ImGui::Text("Magic Resist: %.1f", stats.magicResist);
                ImGui::Text("Attack Speed: %.2f", stats.attackSpeed);
                ImGui::Text("Movement Speed: %.1f", stats.movementSpeed);
                
                // Display hero abilities
                ImGui::Separator();
                ImGui::Text("Abilities:");
                
                const std::vector<std::shared_ptr<Ability>>& abilities = heroComponent->GetAbilities();
                for (size_t i = 0; i < abilities.size(); ++i) {
                    std::shared_ptr<Ability> ability = abilities[i];
                    
                    ImGui::PushID(static_cast<int>(i));
                    
                    if (ImGui::TreeNode(ability->GetAbilityName().c_str())) {
                        ImGui::Text("ID: %s", ability->GetAbilityID().c_str());
                        ImGui::Text("Description: %s", ability->GetDescription().c_str());
                        ImGui::Text("Cooldown: %.1f", ability->GetCooldown());
                        ImGui::Text("Mana Cost: %.1f", ability->GetManaCost());
                        ImGui::Text("Range: %.1f", ability->GetRange());
                        ImGui::Text("Level: %d", ability->GetLevel());
                        
                        // Type-specific details
                        switch (ability->GetAbilityType()) {
                            case Ability::AbilityType::Targeted: {
                                TargetedAbility* targetedAbility = dynamic_cast<TargetedAbility*>(ability.get());
                                if (targetedAbility) {
                                    ImGui::Text("Damage: %.1f", targetedAbility->GetDamage());
                                    ImGui::Text("Damage Type: %s", targetedAbility->IsMagicalDamage() ? "Magical" : "Physical");
                                }
                                break;
                            }
                            
                            case Ability::AbilityType::AreaOfEffect: {
                                AreaOfEffectAbility* aoeAbility = dynamic_cast<AreaOfEffectAbility*>(ability.get());
                                if (aoeAbility) {
                                    ImGui::Text("Radius: %.1f", aoeAbility->GetRadius());
                                    ImGui::Text("Damage: %.1f", aoeAbility->GetDamage());
                                    ImGui::Text("Damage Type: %s", aoeAbility->IsMagicalDamage() ? "Magical" : "Physical");
                                }
                                break;
                            }
                            
                            case Ability::AbilityType::Skillshot: {
                                SkillshotAbility* skillshotAbility = dynamic_cast<SkillshotAbility*>(ability.get());
                                if (skillshotAbility) {
                                    ImGui::Text("Width: %.1f", skillshotAbility->GetWidth());
                                    ImGui::Text("Speed: %.1f", skillshotAbility->GetSpeed());
                                    ImGui::Text("Damage: %.1f", skillshotAbility->GetDamage());
                                    ImGui::Text("Damage Type: %s", skillshotAbility->IsMagicalDamage() ? "Magical" : "Physical");
                                }
                                break;
                            }
                            
                            case Ability::AbilityType::SelfBuff: {
                                SelfBuffAbility* buffAbility = dynamic_cast<SelfBuffAbility*>(ability.get());
                                if (buffAbility) {
                                    ImGui::Text("Duration: %.1f", buffAbility->GetDuration());
                                    
                                    MovementSpeedBuffAbility* msBuffAbility = dynamic_cast<MovementSpeedBuffAbility*>(ability.get());
                                    if (msBuffAbility) {
                                        ImGui::Text("Movement Speed Bonus: %.1f", msBuffAbility->GetMovementSpeedBonus());
                                    }
                                    
                                    AttackDamageBuffAbility* adBuffAbility = dynamic_cast<AttackDamageBuffAbility*>(ability.get());
                                    if (adBuffAbility) {
                                        ImGui::Text("Attack Damage Bonus: %.1f", adBuffAbility->GetAttackDamageBonus());
                                    }
                                }
                                break;
                            }
                            
                            default:
                                break;
                        }
                        
                        ImGui::TreePop();
                    }
                    
                    ImGui::PopID();
                }
                
                // Test buttons
                ImGui::Separator();
                
                if (ImGui::Button("Level Up")) {
                    heroComponent->LevelUp();
                }
                
                ImGui::SameLine();
                
                if (ImGui::Button("Take Damage (100)")) {
                    heroComponent->TakeDamage(100.0f, true);
                }
                
              <response clipped><NOTE>To save on context only part of this file has been shown to you. You should retry this tool after you have searched inside the file with `grep -n` in order to find the line numbers of what you are looking for.</NOTE>