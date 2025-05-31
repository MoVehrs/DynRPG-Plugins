/**
 * @file bare_handed_config.cpp
 * @brief Configuration handling for the BareHanded plugin.
 * @details Loads and manages settings from DynRPG.ini, including actor-to-weapon mappings
 *          and debug options.
 */

/**
 * @namespace BareHandedConfig
 * @brief Contains configuration settings and loading functionality for the BareHanded plugin.
 */
namespace BareHandedConfig
{
    /** @brief Flag to enable configuration debug output. */
    bool enableDebugConfig = false;

    /** @brief Flag to enable runtime debug output. */
    bool enableDebugRuntime = false;
    
    /**
     * @brief Maps actor IDs to fixed bare hand weapon IDs.
     * @details Positive values indicate weapon IDs to equip when the actor is unarmed.
     */
    std::map<int, int> actorWeaponMap;
    
    /**
     * @brief Maps actor IDs to variable IDs containing weapon IDs.
     * @details Variables contain weapon IDs to equip when the actor is unarmed.
     *          Variable values must be positive to be considered valid.
     */
    std::map<int, int> actorVariableMap;
    
    /**
     * @brief Loads and validates configuration settings from DynRPG.ini.
     * @param pluginName Name of the plugin section in DynRPG.ini.
     * @return True if configuration loaded successfully, false otherwise.
     * @details This function:
     *          - Clears existing configuration
     *          - Loads debug settings
     *          - Loads actor-to-weapon mappings
     *          - Validates weapon and variable IDs
     * @note Both fixed weapon IDs and variable IDs must be positive values
     *       to be considered valid configurations.
     */
    bool LoadConfig(char *pluginName) {
        // Reset configuration state
        actorWeaponMap.clear();
        actorVariableMap.clear();
        
        // Load plugin configuration
        std::map<std::string, std::string> config = RPG::loadConfiguration(pluginName);
        
        // Parse debug settings
        enableDebugConfig = false;
        enableDebugRuntime = false;
        
        if (config.find("EnableDebugConfig") != config.end()) {
            enableDebugConfig = (config["EnableDebugConfig"] == "true" || config["EnableDebugConfig"] == "1");
        }
        
        if (config.find("EnableDebugRuntime") != config.end()) {
            enableDebugRuntime = (config["EnableDebugRuntime"] == "true" || config["EnableDebugRuntime"] == "1");
        }
        
        // Parse maximum actor ID to check
        int maxActorId = 20; // Default value
        if (config.find("MaxActorId") != config.end()) {
            try {
                maxActorId = std::stoi(config["MaxActorId"]);
            } catch (...) {
                // Skip invalid actor ID configuration
            }
        }
        
        if (enableDebugConfig && Debug::enableConsole) {
            std::cout << "[BareHanded - Configuration]" << std::endl;
            std::cout << "Loading configuration with MaxActorId=" << maxActorId << std::endl;
            std::cout << std::endl;
        }
        
        // Process actor configurations
        for (int actorId = 1; actorId <= maxActorId; ++actorId) {
            std::stringstream keyPrefix;
            keyPrefix << "Actor" << actorId;
            
            // Process fixed weapon ID configuration
            std::string weaponKey = keyPrefix.str() + "_UnarmedWeaponId";
            if (config.find(weaponKey) != config.end()) {
                int weaponId = 0;
                try {
                    weaponId = std::stoi(config[weaponKey]);
                } catch (...) {
                    // Skip invalid weapon ID configuration
                }
                
                // Store valid weapon IDs
                if (weaponId > 0) {
                    actorWeaponMap[actorId] = weaponId;
                    
                    if (enableDebugConfig && Debug::enableConsole) {
                        std::cout << "[BareHanded - Configuration]" << std::endl;
                        std::cout << "Configured Actor " << actorId << " with UnarmedWeaponId=" << weaponId << std::endl;
                        std::cout << std::endl;
                    }
                }
                else if (enableDebugConfig && Debug::enableConsole) {
                    // Log invalid weapon ID configuration
                    std::cout << "[BareHanded - Configuration Conflict]" << std::endl;
                    std::cout << "Conflict detected: Skipping Actor " << actorId << ": Invalid UnarmedWeaponId=" << weaponId 
                             << " (must be > 0)" << std::endl;
                    std::cout << std::endl;
                }
            }
            
            // Process variable-based weapon ID configuration
            std::string varKey = keyPrefix.str() + "_VariableId";
            bool hasVarKey = (config.find(varKey) != config.end());
            if (hasVarKey) {
                int variableId = 0;
                try {
                    variableId = std::stoi(config[varKey]);
                } catch (...) {
                    // Skip invalid variable ID configuration
                }
                
                // Store valid variable IDs
                if (variableId > 0) {
                    actorVariableMap[actorId] = variableId;
                    
                    if (enableDebugConfig && Debug::enableConsole) {
                        std::cout << "[BareHanded - Configuration]" << std::endl;
                        std::cout << "Configured Actor " << actorId << " with VariableId=" << variableId 
                                 << " (variable-based weapon ID)" << std::endl;
                        std::cout << std::endl;
                    }
                }
                else if (enableDebugConfig && Debug::enableConsole) {
                    // Log invalid variable ID configuration
                    std::cout << "[BareHanded - Configuration Conflict]" << std::endl;
                    std::cout << "Conflict detected: Skipping Actor " << actorId << " variable-based weapon ID: "
                             << "Invalid VariableId=" << variableId << " (must be > 0)" << std::endl;
                    std::cout << std::endl;
                }
            }
        }
        
        return true;
    }
} // namespace BareHandedConfig
