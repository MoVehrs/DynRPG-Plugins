/*
 * Configuration handling for the DynamicQuickPatch plugin.
 * Loads and manages all settings from DynRPG.ini.
 */

namespace DynamicQuickPatchConfig
{
    // Enum for quickpatch value types
    enum QuickPatchType {
        QPTYPE_8BIT,    // 8-bit signed integer (% notation)
        QPTYPE_32BIT,   // 32-bit signed integer (# notation)
        QPTYPE_HEX_RAW  // Raw hex values
    };

    // Structure to define a variable-to-quickpatch mapping
    struct QuickPatchMapping {
        int variableId;           // RPG Maker variable ID to monitor
        unsigned int address;     // Memory address to patch
        QuickPatchType type;      // Type of quickpatch value
        std::string hexValue;     // For HEX_RAW type: hex string without spaces (e.g. "1A2B3C")
        bool applyOnLoadGame;     // Whether to apply this patch when loading a save game
    };

    // ====== CONFIGURATION VARIABLES ======
    // Enable or disable debug message boxes
    static bool showDebugMessages = false;
    
    // Maximum variable ID (default: 1000, can be configured in DynRPG.ini)
    static int maxVariableId = 1000;
    
    // Store the quickpatch mappings
    static std::vector<QuickPatchMapping> quickpatchMappings;
    // ====== END CONFIGURATION VARIABLES ======

    /**
     * @brief Converts a string to an integer with error handling
     * 
     * @param str The string to convert
     * @param defaultValue The value to return if conversion fails
     * @return int The parsed integer or defaultValue if parsing fails
     */
    int stringToInt(const std::string& str, int defaultValue = 0) {
        try {
            if (str.substr(0, 2) == "0x") {
                // Handle hex format
                return std::stoi(str, nullptr, 16);
            }
            return std::stoi(str);
        } catch (...) {
            return defaultValue;
        }
    }

    /**
     * @brief Converts a string to a boolean with error handling
     * 
     * @param str The string to convert
     * @param defaultValue Value to return if conversion fails
     * @return bool The parsed boolean
     */
    bool stringToBool(const std::string& str, bool defaultValue = false) {
        if (str == "true" || str == "1" || str == "yes" || str == "y" || str == "on") {
            return true;
        } else if (str == "false" || str == "0" || str == "no" || str == "n" || str == "off") {
            return false;
        }
        return defaultValue;
    }

    /**
     * @brief Validates a hexadecimal string
     * 
     * @param hexStr The string to validate
     * @return bool True if the string contains only valid hex characters
     */
    bool isValidHexString(const std::string& hexStr) {
        for (char c : hexStr) {
            if (!isxdigit(c)) {
                return false;
            }
        }
        return hexStr.length() % 2 == 0; // Must be even length
    }

    /**
     * @brief Loads plugin configuration from DynRPG.ini
     * 
     * @param pluginName Name of the plugin section in the INI file
     * @return bool True if configuration was loaded successfully
     */
    bool loadConfig(char* pluginName) {
        // Clear any existing configuration
        quickpatchMappings.clear();
        
        // Load configuration dictionary from DynRPG.ini using the plugin name as section
        std::map<std::string, std::string> config = RPG::loadConfiguration(pluginName);
        
        // Load debug settings
        if (config.find("ShowDebugMessages") != config.end()) {
            std::string debugStr = config["ShowDebugMessages"];
            showDebugMessages = stringToBool(debugStr, false);
        } else {
            showDebugMessages = false;
        }
        
        // Show initial debug message if enabled
        if (showDebugMessages) {
            Dialog::ShowInfo("Loading DynamicQuickPatch configuration...", "DynamicQuickPatch");
        }
        
        // Load maximum variable ID setting
        if (config.find("MaxVariableId") != config.end()) {
            std::string maxVarStr = config["MaxVariableId"];
            maxVariableId = stringToInt(maxVarStr, 1000);
            if (maxVariableId <= 0) {
                maxVariableId = 1000; // Fallback to default
            }
        } else {
            maxVariableId = 1000; // Default value
        }
        
        int quickpatchCount = 0;
        bool hasErrors = false;
        std::string errorMessages;
        
        // Process QuickPatch entries from 1 to 100 (arbitrary limit)
        for (int i = 1; i <= 100; i++) {
            std::string prefix = "QuickPatch" + std::to_string(i) + "_";
            
            // Check if this QuickPatch entry exists in the config
            bool entryExists = false;
            for (const auto& pair : config) {
                if (pair.first.find(prefix) == 0) {
                    entryExists = true;
                    break;
                }
            }
            
            // Skip this entry if it doesn't exist in the config
            if (!entryExists) {
                continue;
            }
        
            // Now process this QuickPatch entry
            
            std::string varIdStr, addressStr, typeStr, hexValueStr, onLoadGameStr;
            
            // Get configuration values with appropriate defaults
            if (config.find(prefix + "VariableId") != config.end()) {
                varIdStr = config[prefix + "VariableId"];
            } else {
                varIdStr = "0";
            }
            
            if (config.find(prefix + "Address") != config.end()) {
                addressStr = config[prefix + "Address"];
            } else {
                addressStr = "0";
            }
            
            if (config.find(prefix + "Type") != config.end()) {
                typeStr = config[prefix + "Type"];
            } else {
                typeStr = "";
            }
            
            if (config.find(prefix + "HexValue") != config.end()) {
                hexValueStr = config[prefix + "HexValue"];
            } else {
                hexValueStr = "";
            }
            
            if (config.find(prefix + "OnLoadGame") != config.end()) {
                onLoadGameStr = config[prefix + "OnLoadGame"];
            } else {
                onLoadGameStr = "true";
            }
            
            // Show debug info for this QuickPatch entry if debug is enabled
            if (showDebugMessages) {
                std::stringstream ss;
                ss << "QuickPatch" << i << " Configuration:\n"
                   << "VariableId: " << varIdStr << "\n"
                   << "Address: " << addressStr << "\n"
                   << "Type: " << typeStr << "\n"
                   << "HexValue: " << hexValueStr << "\n"
                   << "OnLoadGame: " << onLoadGameStr;
                Dialog::ShowInfo(ss.str(), "DynamicQuickPatch");
            }
            
            // Skip entries with missing required fields
            if (varIdStr == "0" || addressStr == "0" || typeStr.empty()) {
                if (showDebugMessages) {
                    Dialog::ShowInfo("Skipping " + prefix + " due to missing required fields", "DynamicQuickPatch");
                }
                continue;
            }
            
            // Parse and validate configuration
            int variableId = stringToInt(varIdStr, 0);
            
            // Parse the address - handle both decimal and hexadecimal formats
            unsigned int address = 0;
            if (addressStr.substr(0, 2) == "0x" || addressStr.substr(0, 2) == "0X") {
                // Hexadecimal format
                try {
                    // Convert hex string to unsigned long
                    char* endPtr;
                    address = static_cast<unsigned int>(strtoul(addressStr.c_str(), &endPtr, 16));
                    
                    // Check if conversion was successful
                    if (*endPtr != '\0') {
                        address = 0;
                    }
                } catch (...) {
                    address = 0;
                }
            } else {
                // Decimal format
                address = static_cast<unsigned int>(stringToInt(addressStr, 0));
            }
            
            // Check for configuration errors
            if (variableId <= 0 || variableId > maxVariableId) {
                std::stringstream ss;
                ss << "Error in " << prefix << ": Invalid VariableId '" << varIdStr 
                   << "'. Must be between 1 and " << maxVariableId << ".";
                errorMessages += ss.str() + "\n";
                hasErrors = true;
                continue;
            }
            
            if (address == 0) {
                std::stringstream ss;
                ss << "Error in " << prefix << ": Invalid Address '" << addressStr 
                   << "'. Must be a valid memory address (e.g., 0x496AC7).";
                errorMessages += ss.str() + "\n";
                hasErrors = true;
                continue;
            }
            
            // Determine the type
            QuickPatchType type;
            if (typeStr == "8bit" || typeStr == "%" || typeStr == "8BIT") {
                type = QPTYPE_8BIT;
            } else if (typeStr == "32bit" || typeStr == "#" || typeStr == "32BIT") {
                type = QPTYPE_32BIT;
            } else if (typeStr == "hex" || typeStr == "HEX" || typeStr == "raw" || typeStr == "RAW") {
                type = QPTYPE_HEX_RAW;
                
                // Validate hex string for HEX_RAW type
                if (!isValidHexString(hexValueStr)) {
                    std::stringstream ss;
                    ss << "Error in " << prefix << ": Invalid HexValue '" << hexValueStr 
                       << "'. Must be a valid hex string with even length (e.g., '1A2B3C').";
                    errorMessages += ss.str() + "\n";
                    hasErrors = true;
                    continue;
                }
            } else {
                std::stringstream ss;
                ss << "Error in " << prefix << ": Invalid Type '" << typeStr 
                   << "'. Must be '8bit', '32bit', or 'hex'.";
                errorMessages += ss.str() + "\n";
                hasErrors = true;
                continue;
            }
            
            // Add the valid mapping
            QuickPatchMapping mapping;
            mapping.variableId = variableId;
            mapping.address = address;
            mapping.type = type;
            mapping.hexValue = hexValueStr;
            mapping.applyOnLoadGame = stringToBool(onLoadGameStr, true);
            
            // Debug message to show the mapping being added
            if (showDebugMessages) {
                std::stringstream mappingMsg;
                mappingMsg << "Adding mapping:\n"
                          << "Variable ID: " << variableId << "\n"
                          << "Address: 0x" << std::hex << std::uppercase << address << "\n"
                          << "Type: " << (type == QPTYPE_8BIT ? "8-bit" : (type == QPTYPE_32BIT ? "32-bit" : "Hex")) << "\n"
                          << "Apply on load game: " << (mapping.applyOnLoadGame ? "Yes" : "No");
                if (type == QPTYPE_HEX_RAW) {
                    mappingMsg << "\nHex value: " << hexValueStr;
                }
                Dialog::ShowInfo(mappingMsg.str(), "DynamicQuickPatch");
            }
            
            quickpatchMappings.push_back(mapping);
            quickpatchCount++;
        }
        
        // Show configuration summary if debug is enabled
        if (showDebugMessages) {
            std::stringstream summaryMsg;
            summaryMsg << "Configuration loaded successfully.\n"
                       << "Loaded " << quickpatchCount << " quickpatch mappings.\n"
                       << "Maximum Variable ID: " << maxVariableId << "\n";
            
            if (hasErrors) {
                summaryMsg << "\nWarning: Some entries had errors:\n" << errorMessages;
            }
            
            Dialog::ShowInfo(summaryMsg.str(), "DynamicQuickPatch - Configuration");
        }
        
        // Return true if we have at least one valid mapping
        return quickpatchCount > 0;
    }
    
    /**
     * @brief Gets the list of quickpatch mappings
     * 
     * @return const std::vector<QuickPatchMapping>& Reference to the quickpatch mappings vector
     */
    const std::vector<QuickPatchMapping>& getMappings() {
        return quickpatchMappings;
    }
    
    /**
     * @brief Gets the debug message flag
     * 
     * @return bool True if debug messages are enabled
     */
    bool isDebugEnabled() {
        return showDebugMessages;
    }
    
    /**
     * @brief Gets the maximum variable ID
     * 
     * @return int The maximum variable ID
     */
    int getMaxVariableId() {
        return maxVariableId;
    }
} // namespace DynamicQuickPatchConfig