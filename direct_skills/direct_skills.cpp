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

    /** @brief Tracks the persistent command selection across frames. */
    static int persistentSelectedCommand = -1;

    /** @brief Tracks whether command selection has been initialized. */
    static bool selectionInitialized = false;

    /**
     * @brief Processes frame updates during battle.
     * @param scene The current game scene.
     * @details Tracks the selected command in the battle command window
     *          and maintains persistent command selection state.
     * @note Only processes updates during battle scenes.
     */
    void onFrame(RPG::Scene scene) {
        // Only process updates during battle scenes
        if (scene != RPG::SCENE_BATTLE) return;

        // Validate battle data and command window availability
        if (RPG::battleData && RPG::battleData->winCommand) {
            int currentSelection = RPG::battleData->winCommand->getSelected();

            // Initialize command selection on first valid frame
            if (!selectionInitialized && currentSelection >= 0) {
                lastSelectedCommand = currentSelection;
                persistentSelectedCommand = currentSelection;
                selectionInitialized = true;
                return;
            }

            // Update selection state on command change
            if (currentSelection != lastSelectedCommand) {
                lastSelectedCommand = currentSelection;

                // Persist valid selections only
                if (currentSelection >= 0) {
                    persistentSelectedCommand = currentSelection;
                }
            }
        }
    }

    /**
     * @brief Processes battler actions before they are executed.
     * @param battler Pointer to the battler performing the action.
     * @param firstTry Whether this is the first attempt to process this action.
     * @return True to allow the action to proceed.
     * @details Checks if the current action should be replaced with a skill
     *          based on the selected command and configuration.
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

        // Output debug information for battle commands
        if (DirectSkillsConfig::enableDebugBattle && Debug::enableConsole) {
            std::cout << "[DirectSkills - Debug Info]" << std::endl;
            std::cout << "Actor" << actor->id << "'s Battle Commands:" << std::endl;

            if (actor->battleCommands) {
                for (int i = 0; i < 4; i++) {
                    std::cout << "  Index " << i << ": Command ID " << actor->battleCommands[i];

                    // Display command name if available
                    if (actor->battleCommands[i] > 0) {
                        int commandIndex = actor->battleCommands[i] - 1;
                        if (RPG::battleSettings && commandIndex >= 0 && commandIndex < 100 &&
                            RPG::battleSettings->battleCommands[commandIndex] &&
                            RPG::battleSettings->battleCommands[commandIndex]->name) {
                            std::string cmdName = RPG::battleSettings->battleCommands[commandIndex]->name.s_str();
                            std::cout << " (" << cmdName << ")";
                        }
                    }

                    // Indicate currently selected command
                    if (i == persistentSelectedCommand) {
                        std::cout << " <---";
                    }

                    std::cout << std::endl;
                }
            } else {
                std::cout << "  battleCommands is NULL!" << std::endl;
            }
            std::cout << std::endl;
        }

        // Validate command selection and battle commands
        if (persistentSelectedCommand >= 0 && actor->battleCommands &&
            persistentSelectedCommand < 4 && actor->battleCommands[persistentSelectedCommand] > 0) {

            int selectedCommandId = actor->battleCommands[persistentSelectedCommand];

            // Check if command is mapped to a skill
            if (DirectSkillsConfig::commandToSkillMap.find(selectedCommandId) !=
                DirectSkillsConfig::commandToSkillMap.end()) {

                int skillId = getSkillIdForCommand(selectedCommandId);

                if (skillId > 0) {
                    // Output debug information for action replacement
                    if (DirectSkillsConfig::enableDebugBattle && Debug::enableConsole) {
                        std::string cmdName = "Unknown";
                        int commandIndex = selectedCommandId - 1;
                        if (RPG::battleSettings && commandIndex >= 0 && commandIndex < 100 &&
                            RPG::battleSettings->battleCommands[commandIndex]) {
                            if (RPG::battleSettings->battleCommands[commandIndex]->name) {
                                cmdName = RPG::battleSettings->battleCommands[commandIndex]->name.s_str();
                            }
                        }

                        std::cout << "[DirectSkills - Debug Info]" << std::endl;
                        std::cout << "Action Swapped for Actor" << actor->id << std::endl;
                        std::cout << "Selected Command Index: " << persistentSelectedCommand << std::endl;
                        std::cout << "Battle Command ID: " << selectedCommandId << " (" << cmdName << ")" << std::endl;

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
                    return true;
                }
            } else {
                // Output debug information for unmapped command
                if (DirectSkillsConfig::enableDebugBattle && Debug::enableConsole) {
                    std::cout << "[DirectSkills - Debug Info]" << std::endl;
                    std::cout << "No Mapping Found for Actor" << actor->id << std::endl;
                    std::cout << "Selected Command ID: " << selectedCommandId << std::endl;
                    std::cout << "This command is not in the skill mapping." << std::endl;
                    std::cout << std::endl;
                }
            }
        } else {
            // Output debug information for validation failure
            if (DirectSkillsConfig::enableDebugBattle && Debug::enableConsole) {
                std::cout << "[DirectSkills - Debug Info]" << std::endl;
                std::cout << "Validation Failed for Actor" << actor->id << std::endl;
                std::cout << "Selected Command Index: " << persistentSelectedCommand << std::endl;
                std::cout << "battleCommands is NULL: " << (actor->battleCommands ? "No" : "Yes") << std::endl;
                if (actor->battleCommands && persistentSelectedCommand >= 0 && persistentSelectedCommand < 4) {
                    std::cout << "Command at index " << persistentSelectedCommand << ": "
                             << actor->battleCommands[persistentSelectedCommand] << std::endl;
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
     * @details Resets the persistent selected command to prevent command selection
     *          from bleeding over between different actors' turns.
     */
    bool onBattlerActionDone(RPG::Battler* battler, bool success) {
        // Reset command selection to prevent carry-over between actors
        persistentSelectedCommand = -1;
        return true;
    }
} // namespace DirectSkills
