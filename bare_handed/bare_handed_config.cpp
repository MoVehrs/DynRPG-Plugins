/*
 * Configuration handling for the BareHanded plugin.
 * Loads settings from DynRPG.ini and provides access to them.
 */

namespace BareHandedConfig
{
    // Debug options
    bool enableDebugConfig = false; // Debug messages during configuration loading
    bool enableDebugRuntime = false; // Debug messages during gameplay
    
    // Map to store actor ID to unarmed weapon ID mapping
    std::map<int, int> actorWeaponMap;
    
    // Maps to store actor ID to variable ID mapping for variable-based weapon IDs
    std::map<int, int> actorVariableMap;
    
    /**
     * @brief Loads configuration from DynRPG.ini
     * 
     * @param pluginName Name of the plugin section in DynRPG.ini
     * @return bool True if loaded successfully, false otherwise
     */
    bool LoadConfig(char *pluginName) {
        // Clear any existing configuration
        actorWeaponMap.clear();
        actorVariableMap.clear();
        
        // Load configuration from DynRPG.ini
        std::map<std::string, std::string> config = RPG::loadConfiguration(pluginName);
        
        // Load debug settings
        enableDebugConfig = false;
        enableDebugRuntime = false;
        
        if (config.find("EnableDebugConfig") != config.end()) {
            enableDebugConfig = (config["EnableDebugConfig"] == "true" || config["EnableDebugConfig"] == "1");
        }
        
        if (config.find("EnableDebugRuntime") != config.end()) {
            enableDebugRuntime = (config["EnableDebugRuntime"] == "true" || config["EnableDebugRuntime"] == "1");
        }
        
        // Load the maximum actor ID to check
        int maxActorId = 20; // Default value
        if (config.find("MaxActorId") != config.end()) {
            try {
                maxActorId = std::stoi(config["MaxActorId"]);
            } catch (...) {
                // Use default value if parsing fails
            }
        }
        
        if (enableDebugConfig) {
            std::stringstream ss;
            ss << "Loading configuration with MaxActorId=" << maxActorId;
            Dialog::Show(ss.str(), "BareHanded - Configuration");
        }
        
        // For each possible actor, check if they have a configuration
        for (int actorId = 1; actorId <= maxActorId; ++actorId) {
            std::stringstream keyPrefix;
            keyPrefix << "Actor" << actorId;
            
            // Get unarmed weapon ID for this actor
            std::string weaponKey = keyPrefix.str() + "_UnarmedWeaponId";
            if (config.find(weaponKey) != config.end()) {
                int weaponId = 0;
                try {
                    weaponId = std::stoi(config[weaponKey]);
                } catch (...) {
                    // Use default value if parsing fails
                }
                
                // Only add positive weapon IDs to the map
                if (weaponId > 0) {
                    actorWeaponMap[actorId] = weaponId;
                    
                    if (enableDebugConfig) {
                        std::stringstream ss;
                        ss << "Configured Actor " << actorId << " with UnarmedWeaponId=" << weaponId;
                        Dialog::Show(ss.str(), "BareHanded - Configuration");
                    }
                }
                else {
                    // If a 0 or negative value was explicitly configured, log it
                    if (enableDebugConfig) {
                        std::stringstream ss;
                        ss << "Conflict detected: Skipping Actor " << actorId << ": Invalid UnarmedWeaponId=" << weaponId 
                           << " (must be > 0)";
                        Dialog::Show(ss.str(), "BareHanded - Configuration Conflict");
                    }
                }
            }
            
            // Check for variable-based weapon ID
            std::string varKey = keyPrefix.str() + "_VariableId";
            bool hasVarKey = (config.find(varKey) != config.end());
            if (hasVarKey) {
                int variableId = 0;
                try {
                    variableId = std::stoi(config[varKey]);
                } catch (...) {
                    // Use default value if parsing fails
                }
                
                // Only add valid variable IDs to the map
                if (variableId > 0) {
                    actorVariableMap[actorId] = variableId;
                    
                    if (enableDebugConfig) {
                        std::stringstream ss;
                        ss << "Configured Actor " << actorId << " with VariableId=" << variableId 
                           << " (variable-based weapon ID)";
                        Dialog::Show(ss.str(), "BareHanded - Configuration");
                    }
                }
                else if (enableDebugConfig) {
                    // Invalid variable ID
                    std::stringstream ss;
                    ss << "Conflict detected: Skipping Actor " << actorId << " variable-based weapon ID: "
                       << "Invalid VariableId=" << variableId << " (must be > 0)";
                    Dialog::Show(ss.str(), "BareHanded - Configuration Conflict");
                }
            }
        }
        
        return true;
    }
}
