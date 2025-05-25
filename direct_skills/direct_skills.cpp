/*
 * Main implementation file for the DirectSkills plugin.
 * Contains the primary DirectSkills namespace with all callback implementations.
 * 
 * This plugin allows battle commands with attack archtype to be automatically
 * replaced with specific skills without any additional conditions.
 */

// Include our modular code
#include "dialog.cpp"
#include "direct_skills_config.cpp"

// Main namespace to handle callbacks
namespace DirectSkills
{
    // Global variables to track action state
    bool actionReplaced = false;
    
    // Variables to track default skill ID usage
    bool usingDefaultSkill = false;
    int variableId = 0;
    int invalidVariableValue = 0;
    int defaultSkillId = 0;

    /**
     * @brief Gets the actual skill ID for a command
     * 
     * @param commandId The battle command ID
     * @return int The skill ID to use, or 0 if not found
     * 
     * @note If the stored value is positive, it's a direct skill ID
     *       If the stored value is negative, it's a variable ID containing the skill ID
     */
    int getSkillIdForCommand(int commandId) {
        // Reset default skill tracking variables
        usingDefaultSkill = false;
        variableId = 0;
        invalidVariableValue = 0;
        defaultSkillId = 0;
        
        auto it = DirectSkillsConfig::commandToSkillMap.find(commandId);
        if (it == DirectSkillsConfig::commandToSkillMap.end()) {
            return 0; // Not found
        }
        
        int value = it->second;
        if (value > 0) {
            // Direct skill ID
            return value;
        } else if (value < 0) {
            // Variable ID containing skill ID (stored as negative)
            int varId = -value; // Convert back to positive variable ID
            int variableValue = RPG::variables[varId];
            
            // Check if the variable contains a valid skill ID
            if (variableValue > 0) {
                return variableValue;
            } else {
                // Variable contains invalid skill ID, use default if available
                auto defaultIt = DirectSkillsConfig::defaultSkillMap.find(commandId);
                if (defaultIt != DirectSkillsConfig::defaultSkillMap.end()) {
                    // Store the info about invalid variable for later display
                    // We'll include this in the main action swap message
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
     * @brief Initializes the DirectSkills plugin
     * 
     * @param pluginName Name of the plugin section in DynRPG.ini
     * @return bool True if configuration loaded successfully, false otherwise
     * 
     * @note Called once at plugin startup
     *       Delegates to DirectSkillsConfig::LoadConfig to read all settings
     */
    bool onStartup(char *pluginName) {
        return DirectSkillsConfig::LoadConfig(pluginName);
    }

    /**
     * @brief Processes battler actions before they occur
     * 
     * @param battler Pointer to the battler (actor or monster) performing the action
     * @param firstTry Whether this is the first attempt at processing this action
     * @return bool Always returns true to continue normal processing
     * 
     * @note This function checks if an actor is using a battle command that's
     *       configured to execute a skill instead of the default attack action.
     */
    bool onDoBattlerAction(RPG::Battler* battler, bool firstTry) {
        // Only process on first try and only for actor actions
        if (!firstTry || battler->isMonster()) {
            return true;
        }

        // Cast to actor
        RPG::Actor* actor = reinterpret_cast<RPG::Actor*>(battler);
        if (!actor) {
            return true;
        }

        // Get the current action
        RPG::Action* action = actor->action;
        if (!action) {
            return true;
        }

        // Only proceed if this is a basic attack action (Attack or Double Attack)
        if (action->kind != RPG::AK_BASIC || 
            (action->basicActionId != RPG::BA_ATTACK && 
             action->basicActionId != RPG::BA_DOUBLE_ATTACK)) {
            return true;
        }

        // Find which battle command the actor is using
        // We need to check all battle commands, not just the first one
        int currentCommandId = -1;
        
        // Loop through the actor's battle commands to find which one is being used
        for (int i = 0; i < 7; i++) { // RPG Maker 2003 has a battleCommands[7] array
            // Make sure we have valid battle commands and this index has a valid command ID
            if (actor->battleCommands && actor->battleCommands[i] > 0) {
                // Check if this is the command being used
                // For simplicity, we'll check if this command is configured in our map
                if (DirectSkillsConfig::commandToSkillMap.find(actor->battleCommands[i]) != 
                    DirectSkillsConfig::commandToSkillMap.end()) {
                    
                    // This is a potentially mapped command
                    // Instead of trying to check the actionStatus (which might not be available),
                    // let's use the current command ID directly
                    currentCommandId = actor->battleCommands[i];
                    break;
                }
            }
        }

        // If we found a mapped command, swap the action
        if (currentCommandId > 0) {
            // Get the skill ID for this command (handles both fixed IDs and variable-based IDs)
            int skillId = getSkillIdForCommand(currentCommandId);
            
            // Only proceed if we got a valid skill ID
            if (skillId > 0) {
                
                // Swap the action to use the configured skill
                action->kind = RPG::AK_SKILL;
                action->skillId = skillId;
                
                if (DirectSkillsConfig::enableDebugBattle) {
                    // Get the command name from the battle settings
                    std::string cmdName = "Unknown";
                    // Adjust index by subtracting 1 (command IDs are 1-based, array is 0-based)
                    int commandIndex = currentCommandId - 1;
                    if (RPG::battleSettings && commandIndex >= 0 && commandIndex < 100 && 
                        RPG::battleSettings->battleCommands[commandIndex]) {
                        // Convert DStringPtr to std::string using s_str() method
                        if (RPG::battleSettings->battleCommands[commandIndex]->name) {
                            // Use the explicit s_str() method from the DStringPtr class
                            cmdName = RPG::battleSettings->battleCommands[commandIndex]->name.s_str();
                        }
                    }
                    
                    std::string debugMessage = "DirectSkills Action Swap:\n";
                    debugMessage += "Actor: " + std::to_string(actor->id) + "\n";
                    debugMessage += "Battle Command ID: " + std::to_string(currentCommandId) + " (" + cmdName + ")\n";
                    
                    // If we're using a default skill ID because of an invalid variable value,
                    // include that information in the message
                    if (usingDefaultSkill) {
                        debugMessage += "Variable " + std::to_string(variableId) + " contains invalid value: " + 
                                       std::to_string(invalidVariableValue) + "\n";
                        debugMessage += "Using default skill ID: " + std::to_string(skillId);
                    } else {
                        debugMessage += "Skill ID: " + std::to_string(skillId);
                    }
                    
                    Dialog::Show(debugMessage, "DirectSkills - Action Swapped");
                }
            }
        }

        return true;
    }

}
