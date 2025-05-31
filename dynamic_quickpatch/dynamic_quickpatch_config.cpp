/**
 * @file dynamic_quickpatch_config.cpp
 * @brief Configuration handling for the DynamicQuickPatch plugin.
 * @details Loads and manages settings from DynRPG.ini, including variable mappings
 *          and memory patch definitions.
 */

/**
 * @namespace DynamicQuickPatchConfig
 * @brief Contains configuration settings and loading functionality.
 * @details Manages the loading and validation of plugin settings from DynRPG.ini,
 *          including variable-to-memory mappings and patch configurations.
 */
namespace DynamicQuickPatchConfig
{
    /**
     * @brief Enumeration of supported quickpatch value types.
     * @details Defines the different types of memory values that can be written:
     *          - 8-bit signed integers (% notation)
     *          - 32-bit signed integers (# notation)
     *          - Raw hex values
     */
    enum QuickPatchType {
        QPTYPE_8BIT,    ///< 8-bit signed integer (% notation)
        QPTYPE_32BIT,   ///< 32-bit signed integer (# notation)
        QPTYPE_HEX_RAW  ///< Raw hex values
    };

    /**
     * @brief Structure defining a variable-to-quickpatch mapping.
     * @details Maps an RPG Maker variable to a memory address and defines how
     *          the variable's value should be interpreted when writing to memory.
     */
    struct QuickPatchMapping {
        int variableId;           ///< RPG Maker variable ID to monitor
        unsigned int address;     ///< Memory address to patch
        QuickPatchType type;      ///< Type of quickpatch value
        std::string hexValue;     ///< For HEX_RAW type: hex string without spaces (e.g. "1A2B3C")
        bool applyOnLoadGame;     ///< Whether to apply this patch when loading a save game
    };

    /** @brief Maximum variable ID (default: 1000, configurable in DynRPG.ini) */
    static int maxVariableId = 1000;
    
    /** @brief Vector storing all quickpatch mappings */
    static std::vector<QuickPatchMapping> quickpatchMappings;

    /**
     * @brief Gets the list of quickpatch mappings.
     * @return Reference to the vector of QuickPatchMapping objects.
     * @details Provides read-only access to the loaded quickpatch mappings.
     */
    const std::vector<QuickPatchMapping>& getMappings() {
        return quickpatchMappings;
    }

    /**
     * @brief Gets the maximum variable ID.
     * @return The configured maximum variable ID.
     * @details Returns the maximum variable ID that was set in the configuration,
     *          defaulting to 1000 if not specified.
     */
    int getMaxVariableId() {
        return maxVariableId;
    }

    /**
     * @brief Converts a string to an integer with error handling.
     * @param str The string to convert.
     * @param defaultValue Value to return if conversion fails.
     * @return The parsed integer or defaultValue if parsing fails.
     * @details Handles both decimal and hexadecimal format strings, with
     *          proper error handling for invalid input.
     */
    int stringToInt(const std::string& str, int defaultValue = 0) {
        try {
            // Handle hexadecimal format strings (0x prefix)
            if (str.substr(0, 2) == "0x") {
                return std::stoi(str, nullptr, 16);
            }
            return std::stoi(str);
        } catch (...) {
            return defaultValue;
        }
    }

    /**
     * @brief Validates a hexadecimal string.
     * @param hexStr The string to validate.
     * @return True if the string contains only valid hex characters.
     * @details Verifies that:
     *          1. All characters are valid hexadecimal digits
     *          2. String length is even (complete byte pairs)
     */
    bool isValidHexString(const std::string& hexStr) {
        // Validate each character is a valid hex digit
        for (char c : hexStr) {
            if (!isxdigit(c)) {
                return false;
            }
        }
        // Ensure string length represents complete byte pairs
        return hexStr.length() % 2 == 0;
    }

    /**
     * @brief Loads plugin configuration from DynRPG.ini.
     * @param pluginName Name of the plugin section in the INI file.
     * @return True if configuration was loaded successfully.
     * @details Reads and validates all configuration settings from DynRPG.ini:
     *          - Debug console settings
     *          - Maximum variable ID
     *          - QuickPatch mappings (up to 100 entries)
     * @note Invalid entries are skipped with appropriate debug output.
     */
    bool loadConfig(char* pluginName) {
        // Reset configuration state before loading
        quickpatchMappings.clear();
        
        // Load configuration from DynRPG.ini
        std::map<std::string, std::string> config = RPG::loadConfiguration(pluginName);
        
        // Initialize debug console with default disabled state
        Debug::enableConsole = false;
        if (config.find("EnableConsole") != config.end()) {
            Debug::enableConsole = config["EnableConsole"] == "true";
        }
        
        // Initialize debug console if enabled
        if (Debug::enableConsole) {
            Debug::initConsole();
            
            std::cout << "[DynamicQuickPatch - Configuration]" << std::endl;
            std::cout << "Loading DynamicQuickPatch configuration..." << std::endl;
            std::cout << std::endl;
        }
        
        // Load maximum variable ID with fallback to default
        if (config.find("MaxVariableId") != config.end()) {
            std::string maxVarStr = config["MaxVariableId"];
            maxVariableId = stringToInt(maxVarStr, 1000);
            if (maxVariableId <= 0) {
                maxVariableId = 1000;
            }
        } else {
            maxVariableId = 1000;
        }
        
        int quickpatchCount = 0;
        bool hasErrors = false;
        
        // Process up to 100 QuickPatch entries
        for (int i = 1; i <= 100; i++) {
            std::string prefix = "QuickPatch" + std::to_string(i) + "_";
            
            // Check for existence of any settings with this prefix
            bool entryExists = false;
            for (const auto& pair : config) {
                if (pair.first.find(prefix) == 0) {
                    entryExists = true;
                    break;
                }
            }
            
            // Skip if no settings found for this index
            if (!entryExists) {
                continue;
            }
        
            // Extract configuration values with appropriate defaults
            std::string varIdStr = config.find(prefix + "VariableId") != config.end() 
                ? config[prefix + "VariableId"] : "0";
            
            std::string addressStr = config.find(prefix + "Address") != config.end() 
                ? config[prefix + "Address"] : "0";
            
            std::string typeStr = config.find(prefix + "Type") != config.end() 
                ? config[prefix + "Type"] : "";
            
            std::string hexValueStr = config.find(prefix + "HexValue") != config.end() 
                ? config[prefix + "HexValue"] : "";
            
            std::string onLoadGameStr = config.find(prefix + "OnLoadGame") != config.end() 
                ? config[prefix + "OnLoadGame"] : "true";
            
            // Log configuration details in debug mode
            if (Debug::enableConsole) {
                std::cout << "[DynamicQuickPatch - Configuration]" << std::endl;
                std::cout << "QuickPatch" << i << " Configuration:" << std::endl;
                std::cout << "VariableId: " << varIdStr << std::endl;
                std::cout << "Address: " << addressStr << std::endl;
                std::cout << "Type: " << typeStr << std::endl;
                std::cout << "HexValue: " << hexValueStr << std::endl;
                std::cout << "OnLoadGame: " << onLoadGameStr << std::endl;
                std::cout << std::endl;
            }
            
            // Skip entries missing required fields
            if (varIdStr == "0" || addressStr == "0" || typeStr.empty()) {
                if (Debug::enableConsole) {
                    std::cout << "[DynamicQuickPatch - Configuration]" << std::endl;
                    std::cout << "Skipping " << prefix << " due to missing required fields" << std::endl;
                    std::cout << std::endl;
                }
                continue;
            }
            
            // Validate variable ID range
            int variableId = stringToInt(varIdStr, 0);
            if (variableId <= 0 || variableId > maxVariableId) {
                if (Debug::enableConsole) {
                    std::cout << "[DynamicQuickPatch - Configuration Error]" << std::endl;
                    std::cout << "Error in " << prefix << ": Invalid VariableId '" << varIdStr 
                           << "'. Must be between 1 and " << maxVariableId << "." << std::endl;
                    std::cout << std::endl;
                }
                hasErrors = true;
                continue;
            }
            
            // Parse memory address in hex or decimal format
            unsigned int address = 0;
            if (addressStr.substr(0, 2) == "0x" || addressStr.substr(0, 2) == "0X") {
                try {
                    char* endPtr;
                    address = static_cast<unsigned int>(strtoul(addressStr.c_str(), &endPtr, 16));
                    if (*endPtr != '\0') {
                        if (Debug::enableConsole) {
                            std::cout << "[DynamicQuickPatch - Configuration Error]" << std::endl;
                            std::cout << "Error in " << prefix << ": Invalid hex address format '" << addressStr << "'" << std::endl;
                            std::cout << std::endl;
                        }
                        hasErrors = true;
                        continue;
                    }
                } catch (...) {
                    if (Debug::enableConsole) {
                        std::cout << "[DynamicQuickPatch - Configuration Error]" << std::endl;
                        std::cout << "Error in " << prefix << ": Failed to parse hex address '" << addressStr << "'" << std::endl;
                        std::cout << std::endl;
                    }
                    hasErrors = true;
                    continue;
                }
            } else {
                address = static_cast<unsigned int>(stringToInt(addressStr, 0));
                if (address == 0) {
                    if (Debug::enableConsole) {
                        std::cout << "[DynamicQuickPatch - Configuration Error]" << std::endl;
                        std::cout << "Error in " << prefix << ": Invalid decimal address '" << addressStr << "'" << std::endl;
                        std::cout << std::endl;
                    }
                hasErrors = true;
                continue;
            }
            }
            
            // Parse and validate patch type
            QuickPatchType type;
            if (typeStr == "8bit") {
                type = QPTYPE_8BIT;
            } else if (typeStr == "32bit") {
                type = QPTYPE_32BIT;
            } else if (typeStr == "hex") {
                type = QPTYPE_HEX_RAW;
                // Validate hex string format
                if (!isValidHexString(hexValueStr)) {
                    if (Debug::enableConsole) {
                        std::cout << "[DynamicQuickPatch - Configuration Error]" << std::endl;
                        std::cout << "Invalid hex value in " << prefix << std::endl;
                        std::cout << std::endl;
                    }
                    hasErrors = true;
                    continue;
                }
            } else {
                if (Debug::enableConsole) {
                    std::cout << "[DynamicQuickPatch - Configuration Error]" << std::endl;
                    std::cout << "Invalid type '" << typeStr << "' in " << prefix << std::endl;
                    std::cout << std::endl;
                }
                hasErrors = true;
                continue;
            }
            
            // Parse load game behavior flag
            bool applyOnLoadGame = onLoadGameStr == "true";
            
            // Create and store valid mapping
            QuickPatchMapping mapping;
            mapping.variableId = variableId;
            mapping.address = address;
            mapping.type = type;
            mapping.hexValue = hexValueStr;
            mapping.applyOnLoadGame = applyOnLoadGame;
            
            quickpatchMappings.push_back(mapping);
            quickpatchCount++;
        }
        
        // Log configuration summary
        if (Debug::enableConsole) {
            std::cout << "[DynamicQuickPatch - Configuration Summary]" << std::endl;
            std::cout << "Configuration loaded successfully." << std::endl;
            std::cout << "Loaded " << quickpatchCount << " quickpatch mappings." << std::endl;
            std::cout << "Maximum Variable ID: " << maxVariableId << std::endl;
            if (hasErrors) {
                std::cout << "Warning: Some entries had errors and were skipped." << std::endl;
        }
            std::cout << std::endl;
        }
        
        return true;
    }
} // namespace DynamicQuickPatchConfig
