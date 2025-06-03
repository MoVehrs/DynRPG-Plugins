/**
 * @file direct_skills.cpp
 * @brief Main implementation of the DirectSkills plugin.
 * @details Contains the primary DirectSkills namespace with all callback implementations
 *          and core functionality for mapping battle commands to skills.
 */

#include "direct_skills_debug.cpp"
#include "direct_skills_config.cpp"

/**
 * @namespace DirectSkills
 * @brief Core implementation namespace for the DirectSkills plugin.
 * @details Handles battle command to skill mapping and action replacement
 *          during battle. Provides functionality to automatically replace
 *          basic attacks with specific skills based on configuration.
 */
namespace DirectSkills
{
    /** @brief Tracks whether the current action has been replaced. */
    bool actionReplaced = false;

    /** @brief Tracks whether a default skill is being used. */
    bool usingDefaultSkill = false;

    /** @brief Variable ID being used for skill lookup. */
    int variableId = 0;

    /** @brief Invalid value found in the variable. */
    int invalidVariableValue = 0;

    /** @brief Default skill ID being used as fallback. */
    int defaultSkillId = 0;

    /** @brief Tracks the last logged command for each actor to prevent duplicate debug output. */
    static std::map<int,int> lastLoggedCommandMap;

    /**
     * @brief Resolves the actual skill ID for a given battle command.
     * @param commandId The battle command ID to look up.
     * @return The skill ID to use, or 0 if not found.
     * @details Handles both direct skill mappings and variable-based mappings:
     *          - Positive stored values indicate direct skill IDs
     *          - Negative stored values indicate variable IDs containing skill IDs
     * @note For variable-based mappings, if the variable contains an invalid value,
     *       the function will attempt to use a configured default skill ID.
     * @see DirectSkillsConfig::commandToSkillMap
     * @see DirectSkillsConfig::defaultSkillMap
     */
    int getSkillIdForCommand(int commandId) {
        // Reset tracking variables for default skill handling
        usingDefaultSkill = false;
        variableId = 0;
        invalidVariableValue = 0;
        defaultSkillId = 0;

        auto it = DirectSkillsConfig::commandToSkillMap.find(commandId);
        if (it == DirectSkillsConfig::commandToSkillMap.end()) {
            return 0; // Command not mapped to any skill
        }

        int value = it->second;
        if (value > 0) {
            return value; // Direct skill ID mapping
        } else if (value < 0) {
            // Convert negative value back to positive variable ID
            int varId = -value;
            int variableValue = RPG::variables[varId];

            if (variableValue > 0) {
                return variableValue; // Valid skill ID from variable
            } else {
                // Attempt to use default skill ID for invalid variable value
                auto defaultIt = DirectSkillsConfig::defaultSkillMap.find(commandId);
                if (defaultIt != DirectSkillsConfig::defaultSkillMap.end()) {
                    // Store invalid variable information for debug output
                    usingDefaultSkill = true;
                    variableId = varId;
                    invalidVariableValue = variableValue;
                    defaultSkillId = defaultIt->second;

                    return defaultSkillId;
                }
            }
        }

        return 0; // No valid skill ID found
    }

    /**
     * @brief Initializes the DirectSkills plugin.
     * @param pluginName Name of the plugin section in DynRPG.ini.
     * @return True if initialization succeeded, false otherwise.
     * @details Performs the following initialization steps:
     *          1. Loads configuration from DynRPG.ini
     *          2. Sets up debug console if enabled
     * @see DirectSkillsConfig::LoadConfig
     * @see Debug::initConsole
     */
    bool onStartup(char *pluginName) {
        // Load configuration from DynRPG.ini
        std::map<std::string, std::string> configuration = RPG::loadConfiguration(pluginName);

        // Configure debug console based on settings
        Debug::enableConsole = false;
        if (configuration.find("EnableConsole") != configuration.end()) {
            Debug::enableConsole = configuration["EnableConsole"] == "true";
        }

        // Initialize debug console if enabled
        if (Debug::enableConsole) {
            Debug::initConsole();
        }

        return DirectSkillsConfig::LoadConfig(pluginName);
    }

    /**
     * @brief Performs cleanup when the plugin is being unloaded.
     * @details Cleans up the debug console if it was initialized.
     * @see Debug::cleanupConsole
     */
    void onExit() {
        Debug::cleanupConsole();
    }

    /** @brief Tracks the last selected command index. */
    static int lastSelectedCommand = -1;

    /** @brief Stores the actual command ID of the selected command for each actor. */
    static std::map<int, int> actorCommandMap;

    /** @brief Tracks the last active actor to detect actor changes. */
    static int lastActiveActorId = -1;

    /** @brief Tracks whether command selection has been initialized. */
    static bool selectionInitialized = false;

    /**
     * @brief Processes frame updates during battle.
     * @param scene The current game scene.
     * @details Tracks the selected command in the battle command window
     *          and stores the actual command ID when a valid selection is made.
     *          Maintains separate command storage for each actor and prevents
     *          duplicate debug output by tracking previously logged commands.
     * @note Only processes updates during battle scenes.
     */
    void onFrame(RPG::Scene scene) {
        // Only process updates during battle scenes
        if (scene != RPG::SCENE_BATTLE) return;

        // Ensure battle data and the command window exist
        if (!(RPG::battleData && RPG::battleData->winCommand)) return;

        // Grab the current battler (could be actor or monster)
        RPG::Battler* battler = RPG::battleData->currentHero;
        if (!battler) return;

        // If it's a monster, do nothing
        if (battler->isMonster()) return;

        // Now we know it's an actor; cast so we can read battleCommands[]
        RPG::Actor* currentActor = reinterpret_cast<RPG::Actor*>(battler);
        if (!currentActor) return;

        // If battleCommands pointer is still null, wait for next frame
        if (!currentActor->battleCommands) return;

        // Which "slot" in the command window is selected?
        int currentSelection = RPG::battleData->winCommand->getSelected();
        if (currentSelection < 0 || currentSelection >= 4) {
            // No valid selection index (e.g. command window closed or out of range)
            return;
        }

        // Fetch the actual command ID from battleCommands[]
        int commandId = currentActor->battleCommands[currentSelection];
        if (commandId <= 0) {
            // Either still zero or invalid—means the engine hasn't populated it yet
            return;
        }

        // At this point, we have a "real" commandId (> 0)
        int actorId = currentActor->id;

        // Always update the map so onDoBattlerAction() sees it
        actorCommandMap[actorId] = commandId;

        // Only log to console if this is a new command for this actor
        int prevLogged = 0;
        auto itLogged = lastLoggedCommandMap.find(actorId);
        if (itLogged != lastLoggedCommandMap.end()) {
            prevLogged = itLogged->second;
        }

        if (prevLogged != commandId) {
            // Debug output (once per actual change)
            if (DirectSkillsConfig::enableDebugBattle && Debug::enableConsole) {
                std::cout << "[DirectSkills - Debug Info]" << std::endl;
                std::cout << "Command Selected in Frame:" << std::endl;
                std::cout << "  Actor ID:      " << actorId << std::endl;
                std::cout << "  Command Index: " << currentSelection << std::endl;
                std::cout << "  Command ID:    " << commandId << std::endl;

                // Optionally show the name
                int commandIndex = commandId - 1;
                if (RPG::battleSettings &&
                    commandIndex >= 0 && commandIndex < 100 &&
                    RPG::battleSettings->battleCommands[commandIndex] &&
                    RPG::battleSettings->battleCommands[commandIndex]->name)
                {
                    std::string cmdName = RPG::battleSettings
                                              ->battleCommands[commandIndex]
                                              ->name.s_str();
                    std::cout << "  Command Name:  " << cmdName << std::endl;
                }
                std::cout << std::endl;
            }
            lastLoggedCommandMap[actorId] = commandId;
        }
    }

    /**
     * @brief Processes battler actions before they are executed.
     * @param battler Pointer to the battler performing the action.
     * @param firstTry Whether this is the first attempt to process this action.
     * @return True to allow the action to proceed.
     * @details Checks if the current action should be replaced with a skill
     *          based on the stored command selection and configuration.
     *          Uses the stored command mapping instead of current selection
     *          to prevent timing issues.
     * @note Only processes actor actions on their first attempt.
     * @see getSkillIdForCommand
     */
    bool onDoBattlerAction(RPG::Battler* battler, bool firstTry) {
        // Skip processing for non-first attempts and monster actions
        if (!firstTry || battler->isMonster()) {
            return true;
        }

        // Cast battler to actor and validate
        RPG::Actor* actor = reinterpret_cast<RPG::Actor*>(battler);
        if (!actor) {
            return true;
        }

        // Validate action exists
        RPG::Action* action = actor->action;
        if (!action) {
            return true;
        }

        // Only process basic attack actions
        if (action->kind != RPG::AK_BASIC ||
            (action->basicActionId != RPG::BA_ATTACK &&
             action->basicActionId != RPG::BA_DOUBLE_ATTACK)) {
            return true;
        }

        // Check if we have a stored command selection for this actor
        auto commandIt = actorCommandMap.find(actor->id);
        if (commandIt != actorCommandMap.end()) {
            int storedCommandId = commandIt->second;

            // Output debug information for current state
            if (DirectSkillsConfig::enableDebugBattle && Debug::enableConsole) {
                std::cout << "[DirectSkills - Debug Info]" << std::endl;
                std::cout << "Processing Action for Actor" << actor->id << std::endl;
                std::cout << "Stored Command ID: " << storedCommandId << std::endl;

                // Display current battleCommands state for debugging
                std::cout << "Current battleCommands state:" << std::endl;
                if (actor->battleCommands) {
                    for (int i = 0; i < 4; i++) {
                        std::cout << "  Index " << i << ": Command ID " << actor->battleCommands[i] << std::endl;
                    }
                } else {
                    std::cout << "  battleCommands is NULL!" << std::endl;
                }
                std::cout << std::endl;
            }

            // Check if the stored command is mapped to a skill
            if (DirectSkillsConfig::commandToSkillMap.find(storedCommandId) !=
                DirectSkillsConfig::commandToSkillMap.end()) {

                int skillId = getSkillIdForCommand(storedCommandId);

                if (skillId > 0) {
                    RPG::Skill* skill = RPG::skills[skillId];

                    // Output debug information for action replacement
                    if (DirectSkillsConfig::enableDebugBattle && Debug::enableConsole) {
                        std::string cmdName = "Unknown";
                        int commandIndex = storedCommandId - 1;
                        if (RPG::battleSettings && commandIndex >= 0 && commandIndex < 100 &&
                            RPG::battleSettings->battleCommands[commandIndex]) {
                            if (RPG::battleSettings->battleCommands[commandIndex]->name) {
                                cmdName = RPG::battleSettings->battleCommands[commandIndex]->name.s_str();
                            }
                        }

                        std::cout << "[DirectSkills - Debug Info]" << std::endl;
                        std::cout << "Action Swapped for Actor" << actor->id << std::endl;
                        std::cout << "Battle Command ID: " << storedCommandId << " (" << cmdName << ")" << std::endl;

                        if (usingDefaultSkill) {
                            std::cout << "Variable " << variableId << " contains invalid value: " << invalidVariableValue << std::endl;
                            std::cout << "Using default skill ID: " << skillId << std::endl;
                        } else {
                            std::cout << "Skill ID: " << skillId << std::endl;
                        }
                        std::cout << std::endl;
                    }

                    // Replace basic attack with configured skill
                    action->kind = RPG::AK_SKILL;
                    action->skillId = skillId;


                    // Convert skill target to action target
                    switch (skill->target) {
                        case RPG::SKILL_TARGET_ENEMY:
                            action->target = RPG::TARGET_MONSTER;
                            break;
                        case RPG::SKILL_TARGET_ALL_ENEMIES:
                            action->target = RPG::TARGET_ALL_MONSTERS;
                            break;
                        case RPG::SKILL_TARGET_SELF:
                            action->target = RPG::TARGET_ACTOR;
                            action->targetId = actor->id; // Target self
                            break;
                        case RPG::SKILL_TARGET_ALLY:
                            action->target = RPG::TARGET_ACTOR;
                            break;
                        case RPG::SKILL_TARGET_ALL_ALLIES:
                            action->target = RPG::TARGET_ALL_ACTORS;
                            break;
                        default:
                            // Fallback to monster target if unknown
                            action->target = RPG::TARGET_MONSTER;
                            break;
                    }

                    return true;
                }
            } else {
                // Output debug information for unmapped command
                if (DirectSkillsConfig::enableDebugBattle && Debug::enableConsole) {
                    std::cout << "[DirectSkills - Debug Info]" << std::endl;
                    std::cout << "No Mapping Found for Actor" << actor->id << std::endl;
                    std::cout << "Stored Command ID: " << storedCommandId << std::endl;
                    std::cout << "This command is not in the skill mapping." << std::endl;
                    std::cout << std::endl;
                }
            }
        } else {
            // Output debug information for no stored command
            if (DirectSkillsConfig::enableDebugBattle && Debug::enableConsole) {
                std::cout << "[DirectSkills - Debug Info]" << std::endl;
                std::cout << "No Stored Command for Actor" << actor->id << std::endl;
                std::cout << "Available stored commands:" << std::endl;
                for (const auto& pair : actorCommandMap) {
                    std::cout << "  Actor " << pair.first << ": Command ID " << pair.second << std::endl;
                }
                std::cout << std::endl;
            }
        }

        return true;
    }

    /**
     * @brief Processes cleanup after a battler's action completes.
     * @param battler Pointer to the battler that completed the action.
     * @param success Whether the action was successful.
     * @return True to continue normal processing.
     * @details Retains the stored command mapping for future turns,
     *          allowing the same command selection to persist across
     *          multiple battle actions until a new command is selected.
     */
    bool onBattlerActionDone(RPG::Battler* battler, bool success) {
        // Only process actors, not monsters
        if (!battler->isMonster()) {
            RPG::Actor* actor = reinterpret_cast<RPG::Actor*>(battler);
            if (actor) {
                // Do nothing here – allow command mapping to persist across turns
                // It will be updated when a new command is selected in onFrame()
                if (DirectSkillsConfig::enableDebugBattle && Debug::enableConsole) {
                    std::cout << "[DirectSkills - Debug Info]" << std::endl;
                    std::cout << "Action completed for Actor " << actor->id << std::endl;
                    std::cout << "Command mapping retained for now (not cleared)." << std::endl;
                    std::cout << std::endl;
                }
            }
        }
        return true;
    }
} // namespace DirectSkills
