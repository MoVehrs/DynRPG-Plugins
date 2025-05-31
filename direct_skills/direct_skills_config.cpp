/**
 * @file direct_skills_config.cpp
 * @brief Configuration handling for the DirectSkills plugin.
 * @details Loads and manages settings from DynRPG.ini, including command-to-skill mappings
 *          and debug options.
 */

/**
 * @namespace DirectSkillsConfig
 * @brief Contains configuration settings and loading functionality for the DirectSkills plugin.
 */
namespace DirectSkillsConfig
{
    /** @brief Flag to enable configuration debug output. */
    bool enableDebugConfig = false;

    /** @brief Flag to enable battle debug output. */
    bool enableDebugBattle = false;

    /**
     * @brief Maps battle command IDs to skill IDs or variable IDs.
     * @details Positive values indicate direct skill IDs.
     *          Negative values indicate variable IDs containing skill IDs.
     */
    std::map<int, int> commandToSkillMap;

    /** @brief Maps command IDs to default skill IDs for variable-based mappings. */
    std::map<int, int> defaultSkillMap;

    /** @brief Command ID used by the limit_break plugin (for conflict detection). */
    int limitBreakCommandId = 0;

    /** @brief Ultimate command ID used by the limit_break plugin (for conflict detection). */
    int limitBreakUltimateCommandId = 0;

    /**
     * @brief Loads and validates configuration settings from DynRPG.ini.
     * @param pluginName Name of the plugin section in DynRPG.ini.
     * @return True if configuration loaded successfully, false otherwise.
     * @details This function:
     *          - Clears existing configuration
     *          - Checks for conflicts with the limit_break plugin
     *          - Loads debug settings
     *          - Loads command-to-skill mappings
     *          - Validates variable-based mappings have default skill IDs
     * @note Variable-based mappings require a corresponding default skill ID
     *       to handle cases where the variable contains an invalid value.
     * @warning Command IDs that conflict with the limit_break plugin will be skipped
     *          to prevent interference between plugins.
     */
    bool LoadConfig(char *pluginName) {
        // Reset configuration state
        commandToSkillMap.clear();
        defaultSkillMap.clear();

        // Initialize limit break plugin detection
        limitBreakCommandId = 0;
        limitBreakUltimateCommandId = 0;

        // Load limit_break plugin configuration
        std::map<std::string, std::string> limitBreakConfig = RPG::loadConfiguration(const_cast<char*>("limit_break"));

        // Parse limit break command IDs
        if (limitBreakConfig.find("LimitCommandId") != limitBreakConfig.end()) {
            std::string limitCmdStr = limitBreakConfig["LimitCommandId"];
            try {
                limitBreakCommandId = std::stoi(limitCmdStr);

                if (enableDebugConfig && Debug::enableConsole) {
                    std::cout << "[DirectSkills - Conflict Detection]" << std::endl;
                    std::cout << "Found limit_break plugin configuration:" << std::endl;
                    std::cout << "LimitCommandId: " << limitBreakCommandId << std::endl;
                    std::cout << std::endl;
                }
            } catch (...) {
                // Ignore parsing errors for optional configuration
            }
        }

        // Parse ultimate limit break command ID
        if (limitBreakConfig.find("UltimateLimitCommandId") != limitBreakConfig.end()) {
            std::string ultimateCmdStr = limitBreakConfig["UltimateLimitCommandId"];
            try {
                limitBreakUltimateCommandId = std::stoi(ultimateCmdStr);

                if (enableDebugConfig && Debug::enableConsole) {
                    std::cout << "[DirectSkills - Conflict Detection]" << std::endl;
                    std::cout << "Found limit_break plugin configuration:" << std::endl;
                    std::cout << "UltimateLimitCommandId: " << limitBreakUltimateCommandId << std::endl;
                    std::cout << std::endl;
                }
            } catch (...) {
                // Ignore parsing errors for optional configuration
            }
        }

        // Load direct_skills configuration
        std::map<std::string, std::string> config = RPG::loadConfiguration(pluginName);

        // Parse debug settings
        enableDebugConfig = false;
        enableDebugBattle = false;

        if (config.find("EnableDebugConfig") != config.end()) {
            enableDebugConfig = (config["EnableDebugConfig"] == "true");
        }

        if (config.find("EnableDebugBattle") != config.end()) {
            enableDebugBattle = (config["EnableDebugBattle"] == "true");
        }

        int skippedMappings = 0;

        // Process command-to-skill mappings for all possible command IDs
        for (int cmdId = 1; cmdId <= 100; cmdId++) {
            // Load skill mapping configuration
            std::string configKey = "BattleCommandId" + std::to_string(cmdId);
            std::string value = "";
            if (config.find(configKey) != config.end()) {
                value = config[configKey];
            }

            // Load default skill ID configuration
            std::string defaultKey = "BattleCommandId" + std::to_string(cmdId) + "_DefaultId";
            std::string defaultValue = "";
            if (config.find(defaultKey) != config.end()) {
                defaultValue = config[defaultKey];
            }

            // Process default skill ID configuration
            int defaultId = 0;
            if (!defaultValue.empty()) {
                try {
                    defaultId = std::stoi(defaultValue);
                    if (defaultId > 0) {
                        defaultSkillMap[cmdId] = defaultId;
                    }
                } catch (...) {
                    // Skip invalid default skill IDs
                }
            }

            // Process main skill mapping
            if (!value.empty()) {
                try {
                    // Check for variable-based mapping
                    bool isVariable = false;
                    if (!value.empty() && (value[0] == 'v' || value[0] == 'V')) {
                        isVariable = true;
                        value = value.substr(1); // Remove variable indicator
                    }

                    int id = std::stoi(value);
                    if (id > 0) {
                        bool hasConflict = false;

                        // Check for limit break plugin conflicts
                        if (cmdId == limitBreakCommandId && limitBreakCommandId > 0) {
                            hasConflict = true;
                            skippedMappings++;
                            if (Debug::enableConsole) {
                                std::cout << "[DirectSkills - Configuration Conflict]" << std::endl;
                                std::cout << "Conflict detected: BattleCommandId" << cmdId
                                         << " is also used as LimitCommandId in the limit_break plugin." << std::endl;
                                std::cout << "This mapping will be skipped to avoid conflicts." << std::endl;
                                std::cout << std::endl;
                            }
                        }
                        else if (cmdId == limitBreakUltimateCommandId && limitBreakUltimateCommandId > 0) {
                            hasConflict = true;
                            skippedMappings++;
                            if (Debug::enableConsole) {
                                std::cout << "[DirectSkills - Configuration Conflict]" << std::endl;
                                std::cout << "Conflict detected: BattleCommandId" << cmdId
                                         << " is also used as UltimateLimitCommandId in the limit_break plugin." << std::endl;
                                std::cout << "This mapping will be skipped to avoid conflicts." << std::endl;
                                std::cout << std::endl;
                            }
                        }

                        // Validate variable-based mappings have default IDs
                        if (!hasConflict && isVariable) {
                            auto defaultIt = defaultSkillMap.find(cmdId);
                            if (defaultIt == defaultSkillMap.end()) {
                                skippedMappings++;
                                if (Debug::enableConsole) {
                                    std::cout << "[DirectSkills - Configuration Conflict]" << std::endl;
                                    std::cout << "Conflict detected: Variable-based mapping requires a default skill ID!" << std::endl;
                                    std::cout << "BattleCommandId" << cmdId << "=v" << value << " was found, but" << std::endl;
                                    std::cout << "BattleCommandId" << cmdId << "_DefaultId=XXX is missing." << std::endl;
                                    std::cout << "This mapping will be skipped. Please add a default skill ID." << std::endl;
                                    std::cout << std::endl;
                                }

                                hasConflict = true;
                            }
                        }

                        // Store valid mappings
                        if (!hasConflict) {
                            int storedValue = isVariable ? -id : id;
                            commandToSkillMap[cmdId] = storedValue;

                            if (enableDebugConfig && Debug::enableConsole) {
                                std::cout << "[DirectSkills - Configuration]" << std::endl;
                                std::cout << "Loaded mapping:" << std::endl;
                                std::cout << "Battle Command ID: " << cmdId << std::endl;
                                if (isVariable) {
                                    std::cout << "Variable ID for Skill: " << id << std::endl;

                                    if (defaultId > 0) {
                                        std::cout << "Default Skill ID: " << defaultId << std::endl;
                                    }
                                } else {
                                    std::cout << "Fixed Skill ID: " << id << std::endl;
                                }
                                std::cout << std::endl;
                            }
                        }
                    }
                } catch (...) {
                    // Skip invalid skill mappings
                }
            }
        }

        // Output configuration summary
        if (enableDebugConfig && Debug::enableConsole) {
            std::cout << "[DirectSkills - Configuration]" << std::endl;
            std::cout << "Configuration loaded successfully." << std::endl;
            std::cout << "Total command mappings: " << commandToSkillMap.size() << std::endl;

            if (skippedMappings > 0) {
                std::cout << "Skipped mappings: " << skippedMappings << std::endl;
            }
            std::cout << std::endl;
        }

        return true;
    }
} // namespace DirectSkillsConfig
