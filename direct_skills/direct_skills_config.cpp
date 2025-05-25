/*
 * Configuration handling for the DirectSkills plugin.
 * Loads settings from DynRPG.ini and provides access to them.
 */

namespace DirectSkillsConfig
{
    // Debug options
    bool enableDebugConfig = false;
    bool enableDebugBattle = false;
    
    // Map to store battle command ID to skill ID mapping (positive values)
    // or to variable ID containing skill ID (negative values)
    std::map<int, int> commandToSkillMap;
    
    // Map to store default skill IDs for variable-based mappings
    std::map<int, int> defaultSkillMap;
    
    // Storage for limit_break plugin command IDs (for conflict detection)
    int limitBreakCommandId = 0;
    int limitBreakUltimateCommandId = 0;
    
    /**
     * @brief Loads configuration from DynRPG.ini
     * 
     * @param pluginName Name of the plugin section in DynRPG.ini
     * @return bool True if loaded successfully, false otherwise
     */
    bool LoadConfig(char *pluginName) {
        // Clear any existing configuration
        commandToSkillMap.clear();
        defaultSkillMap.clear();
        
        // Check for limit_break plugin configuration to avoid conflicts
        // This is only relevant if both plugins are active
        limitBreakCommandId = 0;
        limitBreakUltimateCommandId = 0;
        
        // Load limit_break configuration to check for conflicts
        std::map<std::string, std::string> limitBreakConfig = RPG::loadConfiguration("limit_break");
        
        // Try to read LimitCommandId from limit_break section
        if (limitBreakConfig.find("LimitCommandId") != limitBreakConfig.end()) {
            std::string limitCmdStr = limitBreakConfig["LimitCommandId"];
            try {
                limitBreakCommandId = std::stoi(limitCmdStr);
                
                if (enableDebugConfig) {
                    std::string debugMessage = "Found limit_break plugin configuration:\n";
                    debugMessage += "LimitCommandId: " + std::to_string(limitBreakCommandId);
                    Dialog::Show(debugMessage, "DirectSkills - Conflict Detection");
                }
            } catch (...) {
                // Ignore parsing errors
            }
        }
        
        // Try to read UltimateLimitCommandId from limit_break section
        if (limitBreakConfig.find("UltimateLimitCommandId") != limitBreakConfig.end()) {
            std::string ultimateCmdStr = limitBreakConfig["UltimateLimitCommandId"];
            try {
                limitBreakUltimateCommandId = std::stoi(ultimateCmdStr);
                
                if (enableDebugConfig) {
                    std::string debugMessage = "Found limit_break plugin configuration:\n";
                    debugMessage += "UltimateLimitCommandId: " + std::to_string(limitBreakUltimateCommandId);
                    Dialog::Show(debugMessage, "DirectSkills - Conflict Detection");
                }
            } catch (...) {
                // Ignore parsing errors
            }
        }
        
        // Load plugin configuration
        std::map<std::string, std::string> config = RPG::loadConfiguration(pluginName);
        
        // Load debug flags
        enableDebugConfig = false;
        enableDebugBattle = false;
        
        if (config.find("EnableDebugConfig") != config.end()) {
            enableDebugConfig = (config["EnableDebugConfig"] == "true");
        }
        
        if (config.find("EnableDebugBattle") != config.end()) {
            enableDebugBattle = (config["EnableDebugBattle"] == "true");
        }
        
        // Counter for skipped mappings
        int skippedMappings = 0;
        
        // Load command-to-skill mappings
        // Format: BattleCommandIdX=Y where X is battle command ID and Y is skill ID
        for (int cmdId = 1; cmdId <= 100; cmdId++) {
            // Load the main skill mapping
            std::string configKey = "BattleCommandId" + std::to_string(cmdId);
            std::string value = "";
            if (config.find(configKey) != config.end()) {
                value = config[configKey];
            }
            
            // Also check for default skill ID for variable-based mappings
            std::string defaultKey = "BattleCommandId" + std::to_string(cmdId) + "_DefaultId";
            std::string defaultValue = "";
            if (config.find(defaultKey) != config.end()) {
                defaultValue = config[defaultKey];
            }
            
            // Process default skill ID for variable-based mappings
            int defaultId = 0;
            if (!defaultValue.empty()) {
                try {
                    defaultId = std::stoi(defaultValue);
                    if (defaultId > 0) {
                        defaultSkillMap[cmdId] = defaultId;
                    }
                } catch (...) {
                    // Skip invalid entries
                }
            }
            
            // Process main skill mapping
            if (!value.empty()) {
                try {
                    // Check if value starts with 'v' or 'V' to indicate a variable
                    bool isVariable = false;
                    if (!value.empty() && (value[0] == 'v' || value[0] == 'V')) {
                        isVariable = true;
                        value = value.substr(1); // Remove the 'v' prefix
                    }
                    
                    int id = std::stoi(value);
                    if (id > 0) {
                        // Check for conflicts or issues with this mapping
                        bool hasConflict = false;
                        
                        // First check for conflicts with limit_break plugin command IDs
                        if (cmdId == limitBreakCommandId && limitBreakCommandId > 0) {
                            hasConflict = true;
                            skippedMappings++;
                            std::string errorMsg = "Conflict detected: BattleCommandId" + std::to_string(cmdId) + 
                                                  " is also used as LimitCommandId in the limit_break plugin.\n" +
                                                  "This mapping will be skipped to avoid conflicts.";
                            Dialog::Show(errorMsg, "DirectSkills - Configuration Conflict");
                        }
                        else if (cmdId == limitBreakUltimateCommandId && limitBreakUltimateCommandId > 0) {
                            hasConflict = true;
                            skippedMappings++;
                            std::string errorMsg = "Conflict detected: BattleCommandId" + std::to_string(cmdId) + 
                                                  " is also used as UltimateLimitCommandId in the limit_break plugin.\n" +
                                                  "This mapping will be skipped to avoid conflicts.";
                            Dialog::Show(errorMsg, "DirectSkills - Configuration Conflict");
                        }
                        
                        // Then check if variable mapping has required default ID
                        if (!hasConflict && isVariable) {
                            // For variable-based mappings, a default skill ID is required
                            // Check if we have a corresponding default skill ID entry
                            auto defaultIt = defaultSkillMap.find(cmdId);
                            if (defaultIt == defaultSkillMap.end()) {
                                // No default skill ID found, show error and skip this mapping
                                skippedMappings++;
                                std::string errorMsg = "Conflict detected: Variable-based mapping requires a default skill ID!\n";
                                errorMsg += "BattleCommandId" + std::to_string(cmdId) + "=" + "v" + value + " was found, but\n";
                                errorMsg += "BattleCommandId" + std::to_string(cmdId) + "_DefaultId=XXX is missing.\n\n";
                                errorMsg += "This mapping will be skipped. Please add a default skill ID.";
                                Dialog::Show(errorMsg, "DirectSkills - Configuration Conflict");
                                
                                // Skip this mapping
                                hasConflict = true;
                            }
                        }
                        
                        // Only store the mapping if there's no conflict
                        if (!hasConflict) {
                            // Store variable IDs as negative values to distinguish them from direct skill IDs
                            int storedValue = isVariable ? -id : id;
                            commandToSkillMap[cmdId] = storedValue;
                        
                            if (enableDebugConfig) {
                                // Note: During configuration loading, RPG::battleSettings may not be fully initialized yet,
                                // so we won't try to get command names.
                                // The battle command names will be properly displayed in debug messages during battle.
                                
                                std::string debugMessage = "Loaded mapping:\n";
                                debugMessage += "Battle Command ID: " + std::to_string(cmdId) + "\n";
                                if (isVariable) {
                                    debugMessage += "Variable ID for Skill: " + std::to_string(id);
                                    
                                    // If we have a default skill ID for this variable mapping, show it
                                    if (defaultId > 0) {
                                        debugMessage += "\nDefault Skill ID: " + std::to_string(defaultId);
                                    }
                                } else {
                                    debugMessage += "Fixed Skill ID: " + std::to_string(id);
                                }
                                Dialog::Show(debugMessage, "DirectSkills - Configuration");
                            }
                        }
                    }
                } catch (...) {
                    // Skip invalid entries
                }
            }
        }
        
        if (enableDebugConfig) {
            std::stringstream ss;
            ss << "Configuration loaded successfully.\n";
            ss << "Total command mappings: " << commandToSkillMap.size();
            
            // Only show skipped mappings if there were any
            if (skippedMappings > 0) {
                ss << "\nSkipped mappings: " << skippedMappings;
            }
            
            Dialog::Show(ss.str(), "DirectSkills - Configuration");
        }
        
        return true;
    }
}
