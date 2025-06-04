/**
 * @file dynamic_quickpatch.cpp
 * @brief Main implementation of the DynamicQuickPatch plugin.
 * @details Contains the primary DynamicQuickPatch namespace with all callback implementations
 *          and core functionality for managing dynamic memory patching during gameplay.
 */

#include "dynamic_quickpatch_debug.cpp"
#include "dynamic_quickpatch_config.cpp"

/**
 * @namespace DynamicQuickPatch
 * @brief Core implementation namespace for the DynamicQuickPatch plugin.
 * @details Handles dynamic memory patching and variable monitoring during gameplay.
 *          Provides functionality to automatically update memory values based on
 *          RPG Maker variable changes and game state.
 */
namespace DynamicQuickPatch
{
    /** @brief Tracks when a game has been loaded. */
    static bool gameJustLoaded = false;

    /** @brief Minimum value for 8-bit signed integers. */
    const int DQP_INT8_MIN = -127;
    /** @brief Maximum value for 8-bit signed integers. */
    const int DQP_INT8_MAX = 127;
    /** @brief Minimum value for 32-bit signed integers. */
    const int DQP_INT32_MIN = std::numeric_limits<int>::min();
    /** @brief Maximum value for 32-bit signed integers. */
    const int DQP_INT32_MAX = std::numeric_limits<int>::max();

    /**
     * @brief Map storing original memory values before patching.
     * @details Key: memory address, Value: vector of original bytes.
     *          Used to restore original memory state when patches are disabled.
     */
    static std::map<unsigned int, std::vector<unsigned char>> originalMemoryValues;

    /**
     * @brief Checks if original memory values are stored.
     * @param address Memory address to check.
     * @param length Number of bytes to check.
     * @return True if values are stored for this address range.
     * @details Verifies if original memory values exist for the specified address
     *          range before performing memory operations.
     */
    bool hasOriginalValues(unsigned int address, size_t length) {
        return originalMemoryValues.find(address) != originalMemoryValues.end() &&
               originalMemoryValues[address].size() >= length;
    }

    /**
     * @brief Stores original memory values before patching.
     * @param address Memory address to store.
     * @param length Number of bytes to store.
     * @details Saves the original memory values before applying patches to allow
     *          restoration of the original state when needed.
     * @warning Will not store values if address is invalid or memory access fails.
     */
    void storeOriginalValues(unsigned int address, size_t length) {
        // Skip if values already exist
        if (hasOriginalValues(address, length)) {
            return;
        }

        // Validate memory address range
        if (address == 0 || address >= 0xFFFFFFFF - length) {
            if (Debug::enableConsole) {
                std::cout << "[DynamicQuickPatch - Memory Error]" << std::endl;
                std::cout << "Invalid memory address: 0x" << std::hex << std::uppercase << address << std::endl;
                std::cout << std::endl;
            }
            return;
        }

        try {
            // Read current memory values
            std::vector<unsigned char> originalBytes;
            for (size_t i = 0; i < length; i++) {
                originalBytes.push_back(*((unsigned char*)(address + i)));
            }
            originalMemoryValues[address] = originalBytes;

            if (Debug::enableConsole) {
                std::cout << "[DynamicQuickPatch - Memory]" << std::endl;
                std::cout << "Stored original memory values at 0x" << std::hex << std::uppercase << address
                         << " (" << std::dec << length << " bytes)" << std::endl;
                std::cout << std::endl;
            }
        } catch (...) {
            if (Debug::enableConsole) {
                std::cout << "[DynamicQuickPatch - Memory Error]" << std::endl;
                std::cout << "Memory access violation at 0x" << std::hex << std::uppercase << address << std::endl;
                std::cout << std::endl;
            }
            return;
        }
    }

    /**
     * @brief Restores original memory values.
     * @param address Memory address to restore.
     * @return True if restore was successful.
     * @details Restores the original memory values that were saved before patching.
     * @warning Will fail if address is invalid or memory access fails.
     */
    bool restoreOriginalValues(unsigned int address) {
        // Verify original values exist
        if (originalMemoryValues.find(address) == originalMemoryValues.end()) {
            return false;
        }

        // Validate memory address range
        if (address == 0 || address >= 0xFFFFFFFF - originalMemoryValues[address].size()) {
            if (Debug::enableConsole) {
                std::cout << "[DynamicQuickPatch - Memory Error]" << std::endl;
                std::cout << "Invalid memory address for restore: 0x" << std::hex << std::uppercase << address << std::endl;
                std::cout << std::endl;
            }
            return false;
        }

        bool success = false;
        try {
            // Write original values back to memory
            const auto& originalBytes = originalMemoryValues[address];
            for (size_t i = 0; i < originalBytes.size(); i++) {
                *((unsigned char*)(address + i)) = originalBytes[i];
            }
            success = true;
        } catch (...) {
            if (Debug::enableConsole) {
                std::cout << "[DynamicQuickPatch - Memory Error]" << std::endl;
                std::cout << "Memory access violation during restore at 0x" << std::hex << std::uppercase << address << std::endl;
                std::cout << std::endl;
            }
            return false;
        }

        return success;
    }

    /**
     * @brief Writes an 8-bit signed integer to memory.
     * @param address Memory address to write to.
     * @param value The value to write.
     * @details Writes an 8-bit value to memory, storing the original value first
     *          if not already stored.
     * @warning Will not write if address is invalid or memory access fails.
     * @see storeOriginalValues
     */
    void write8bitValue(unsigned int address, int value) {
        // Validate memory address
        if (address == 0 || address >= 0xFFFFFFFF) {
            if (Debug::enableConsole) {
                std::cout << "[DynamicQuickPatch - Memory Error]" << std::endl;
                std::cout << "Invalid memory address for 8-bit write: 0x" << std::hex << std::uppercase << address << std::endl;
                std::cout << std::endl;
            }
            return;
        }

        // Store original value if needed
        if (!hasOriginalValues(address, 1)) {
            storeOriginalValues(address, 1);
        }

        try {
            // Write new value to memory
            *((char*)address) = (char)value;
        } catch (...) {
            if (Debug::enableConsole) {
                std::cout << "[DynamicQuickPatch - Memory Error]" << std::endl;
                std::cout << "Memory access violation during 8-bit write at 0x" << std::hex << std::uppercase << address << std::endl;
                std::cout << std::endl;
            }
        }
    }

    /**
     * @brief Writes a 32-bit signed integer to memory.
     * @param address Memory address to write to.
     * @param value The value to write.
     * @details Writes a 32-bit value to memory, storing the original value first
     *          if not already stored.
     * @warning Will not write if address is invalid or memory access fails.
     * @see storeOriginalValues
     */
    void write32bitValue(unsigned int address, int value) {
        // Validate memory address
        if (address == 0 || address >= 0xFFFFFFFF - sizeof(int)) {
            if (Debug::enableConsole) {
                std::cout << "[DynamicQuickPatch - Memory Error]" << std::endl;
                std::cout << "Invalid memory address for 32-bit write: 0x" << std::hex << std::uppercase << address << std::endl;
                std::cout << std::endl;
            }
            return;
        }

        // Store original value if needed
        if (!hasOriginalValues(address, sizeof(int))) {
            storeOriginalValues(address, sizeof(int));
        }

        try {
            // Write new value to memory
            *((int*)address) = value;
        } catch (...) {
            if (Debug::enableConsole) {
                std::cout << "[DynamicQuickPatch - Memory Error]" << std::endl;
                std::cout << "Memory access violation during 32-bit write at 0x" << std::hex << std::uppercase << address << std::endl;
                std::cout << std::endl;
            }
        }
    }

    /**
     * @brief Writes raw hex values to memory.
     * @param address Memory address to write to.
     * @param hexString Hexadecimal string to convert and write.
     * @details Converts a hex string to bytes and writes them to memory,
     *          storing the original values first if not already stored.
     * @warning Will not write if address is invalid or memory access fails.
     * @see storeOriginalValues
     */
    void writeHexValue(unsigned int address, const std::string& hexString) {
        // Calculate number of bytes to write
        size_t byteCount = hexString.length() / 2;

        // Store original values if needed
        if (!hasOriginalValues(address, byteCount)) {
            storeOriginalValues(address, byteCount);
        }

        // Validate memory address range
        if (address == 0 || address >= 0xFFFFFFFF - byteCount) {
            if (Debug::enableConsole) {
                std::cout << "[DynamicQuickPatch - Memory Error]" << std::endl;
                std::cout << "Invalid memory address for hex write: 0x" << std::hex << std::uppercase << address << std::endl;
                std::cout << std::endl;
            }
            return;
        }

        try {
            // Convert hex string to bytes and write to memory
            for (size_t i = 0; i < hexString.length(); i += 2) {
                // Skip incomplete byte pairs
                if (i + 1 >= hexString.length()) break;

                // Convert hex pair to byte value
                std::string byteStr = hexString.substr(i, 2);
                unsigned char byte = (unsigned char)strtol(byteStr.c_str(), NULL, 16);

                // Write byte to memory
                *((unsigned char*)(address + (i/2))) = byte;
            }
        } catch (...) {
            if (Debug::enableConsole) {
                std::cout << "[DynamicQuickPatch - Memory Error]" << std::endl;
                std::cout << "Memory access violation during hex write at 0x" << std::hex << std::uppercase << address << std::endl;
                std::cout << std::endl;
            }
        }
    }

    /**
     * @brief Validates if a value is within range for a specific type.
     * @param value The value to validate.
     * @param type The type to validate against.
     * @param variableId The variable ID (for error messages).
     * @param address The memory address (for error messages).
     * @return True if valid, false if out of range.
     * @details Checks if a value is within the valid range for its type:
     *          - 8-bit: -127 to 127
     *          - 32-bit: Full signed int range
     *          - Hex: No range validation
     */
    bool validateValue(int value, DynamicQuickPatchConfig::QuickPatchType type,
                       int variableId, unsigned int address) {
        bool isValid = true;

        switch (type) {
            case DynamicQuickPatchConfig::QPTYPE_8BIT:
                // Check 8-bit signed range
                if (value < DQP_INT8_MIN || value > DQP_INT8_MAX) {
                    isValid = false;
                    if (Debug::enableConsole) {
                        std::cout << "[DynamicQuickPatch - Range Warning]" << std::endl;
                        std::cout << "Value " << value << " is out of range for 8-bit type!" << std::endl;
                        std::cout << "Variable: " << variableId << std::endl;
                        std::cout << "Address: 0x" << std::hex << std::uppercase << address << std::endl;
                        std::cout << "Valid range: " << std::dec << DQP_INT8_MIN << " to " << DQP_INT8_MAX << std::endl;
                        std::cout << "Value will be clamped to fit in range." << std::endl;
                        std::cout << std::endl;
                    }
                }
                break;

            case DynamicQuickPatchConfig::QPTYPE_32BIT:
                // Check 32-bit signed range
                if (value < DQP_INT32_MIN || value > DQP_INT32_MAX) {
                    isValid = false;
                    if (Debug::enableConsole) {
                        std::cout << "[DynamicQuickPatch - Range Error]" << std::endl;
                        std::cout << "Value " << value << " is out of range for 32-bit type!" << std::endl;
                        std::cout << "Variable: " << variableId << std::endl;
                        std::cout << "Address: 0x" << std::hex << std::uppercase << address << std::endl;
                        std::cout << "Value may cause undefined behavior." << std::endl;
                        std::cout << std::endl;
                    }
                }
                break;

            case DynamicQuickPatchConfig::QPTYPE_HEX_RAW:
                // No range validation for hex values
                break;
        }

        return isValid;
    }

    /**
     * @brief Updates a quickpatch mapping with a new value.
     * @param mapping The mapping definition to update.
     * @param value The new value to apply.
     * @details Applies the new value to memory according to the mapping type:
     *          - For 8-bit: Allows 0 as valid value, clamps to valid range
     *          - For 32-bit: Allows 0 as valid value, direct value write
     *          - For hex: Uses predefined hex string, 0 deactivates patch
     * @note For hex patches, if value is 0, restores original memory values.
     *       For 8-bit and 32-bit patches, 0 is treated as a valid value to write.
     */
    void updateQuickPatch(const DynamicQuickPatchConfig::QuickPatchMapping& mapping, int value) {
        // Handle patch deactivation - only for hex patches
        if (value == 0 && mapping.type == DynamicQuickPatchConfig::QPTYPE_HEX_RAW) {
            // Determine patch size for hex type
            size_t size = mapping.hexValue.length() / 2;
            std::string typeStr = "Raw Hex";

            // Restore original memory values if available
            bool restored = false;
            if (hasOriginalValues(mapping.address, size)) {
                restored = restoreOriginalValues(mapping.address);
            }

            if (Debug::enableConsole) {
                std::cout << "[DynamicQuickPatch - Patch Disabled]" << std::endl;
                std::cout << "QuickPatch Disabled" << std::endl;
                std::cout << "Variable: " << mapping.variableId << std::endl;
                std::cout << "Address: 0x" << std::hex << std::uppercase << mapping.address << std::endl;
                std::cout << "Type: " << typeStr << std::endl;
                if (restored) {
                    std::cout << "Original memory values restored (variable = 0)" << std::endl;
                } else {
                    std::cout << "Patch is now inactive (variable = 0)" << std::endl;
                    std::cout << "Note: Original values were not available to restore" << std::endl;
                }
                std::cout << std::endl;
            }
            return;
        }

        // Read current memory value for logging
        std::string oldValueStr;
        switch (mapping.type) {
            case DynamicQuickPatchConfig::QPTYPE_8BIT: {
                char oldVal = *((char*)mapping.address);
                oldValueStr = "%" + std::to_string((int)oldVal);
                break;
            }
            case DynamicQuickPatchConfig::QPTYPE_32BIT: {
                int oldVal = *((int*)mapping.address);
                oldValueStr = "#" + std::to_string(oldVal);
                break;
            }
            case DynamicQuickPatchConfig::QPTYPE_HEX_RAW: {
                // Format current memory as hex string
                std::string hex;
                size_t hexLen = mapping.hexValue.length();
                for (size_t i = 0; i < hexLen; i += 2) {
                    unsigned char byte = *((unsigned char*)(mapping.address + (i / 2)));
                    char buf[3];
                    sprintf(buf, "%02X", byte);
                    hex += buf;
                }
                oldValueStr = hex;
                break;
            }
        }

        // Validate and adjust value if needed (for non-hex types)
        if (mapping.type != DynamicQuickPatchConfig::QPTYPE_HEX_RAW) {
            validateValue(value, mapping.type, mapping.variableId, mapping.address);
        }
        int adjustedValue = value;

        // Apply new value with range clamping
        switch (mapping.type) {
            case DynamicQuickPatchConfig::QPTYPE_8BIT:
                // Clamp value to 8-bit range (0 is valid)
                if (adjustedValue < DQP_INT8_MIN) adjustedValue = DQP_INT8_MIN;
                if (adjustedValue > DQP_INT8_MAX) adjustedValue = DQP_INT8_MAX;
                write8bitValue(mapping.address, adjustedValue);
                break;

            case DynamicQuickPatchConfig::QPTYPE_32BIT:
                // Direct write (0 is valid)
                write32bitValue(mapping.address, adjustedValue);
                break;

            case DynamicQuickPatchConfig::QPTYPE_HEX_RAW:
                // For hex patches, write the predefined hex value
                writeHexValue(mapping.address, mapping.hexValue);
                break;
        }

        // Log memory update details
        if (Debug::enableConsole) {
            // Format display values
            std::string typeStr;
            std::string newValueStr;
            std::string rangeInfo;

            switch (mapping.type) {
                case DynamicQuickPatchConfig::QPTYPE_8BIT:
                    typeStr = "8-bit (%)";
                    newValueStr = "%" + std::to_string(adjustedValue);
                    if (value != adjustedValue) {
                        rangeInfo = "\nOriginal value: " + std::to_string(value) + " (clamped to range)";
                    }
                    break;

                case DynamicQuickPatchConfig::QPTYPE_32BIT:
                    typeStr = "32-bit (#)";
                    newValueStr = "#" + std::to_string(adjustedValue);
                    break;

                case DynamicQuickPatchConfig::QPTYPE_HEX_RAW:
                    typeStr = "Raw Hex";
                    newValueStr = mapping.hexValue;
                    rangeInfo = "\nPatch is now active (variable > 0)";
                    break;
            }

            std::cout << "[DynamicQuickPatch - Memory Update]" << std::endl;
            std::cout << "QuickPatch Updated" << std::endl;
            std::cout << "Variable: " << mapping.variableId << std::endl;
            std::cout << "Address: 0x" << std::hex << std::uppercase << mapping.address << std::endl;
            std::cout << "Type: " << typeStr << std::endl;
            std::cout << "Old Value: " << oldValueStr << std::endl;
            std::cout << "New Value: " << newValueStr << std::endl;
            if (!rangeInfo.empty()) {
                std::cout << rangeInfo << std::endl;
            }
            std::cout << std::endl;
        }
    }

    /**
     * @brief Plugin initialization handler.
     * @param pluginName Name of the plugin section in DynRPG.ini.
     * @return True if initialization succeeded.
     * @details Loads configuration from DynRPG.ini and initializes debug output
     *          if enabled in the configuration.
     */
    bool onStartup(char *pluginName) {
        // Load plugin configuration
        std::map<std::string, std::string> configuration = RPG::loadConfiguration(pluginName);

        // Initialize debug console with default disabled state
        Debug::enableConsole = false;
        if (configuration.find("EnableConsole") != configuration.end()) {
            Debug::enableConsole = configuration["EnableConsole"] == "true";
        }

        // Initialize debug console if enabled
        if (Debug::enableConsole) {
            Debug::initConsole();
        }

        // Load and validate configuration settings
        return DynamicQuickPatchConfig::loadConfig(pluginName);
    }

    /**
     * @brief Handles starting a new game or returning to title screen.
     * @details Restores all original memory values to prevent patches from
     *          persisting across game sessions.
     */
    void onNewGame() {
        // Track restoration results
        int restoredCount = 0;
        int failedCount = 0;

        // Get unique addresses from mappings
        const auto& mappings = DynamicQuickPatchConfig::getMappings();
        std::set<unsigned int> processedAddresses;

        for (const auto& mapping : mappings) {
            // Skip already processed addresses
            if (processedAddresses.find(mapping.address) != processedAddresses.end()) {
                continue;
            }

            // Mark address as processed
            processedAddresses.insert(mapping.address);

            // Calculate patch size based on type
            size_t size = 0;
            switch (mapping.type) {
                case DynamicQuickPatchConfig::QPTYPE_8BIT:
                    size = 1;
                    break;

                case DynamicQuickPatchConfig::QPTYPE_32BIT:
                    size = 4;
                    break;

                case DynamicQuickPatchConfig::QPTYPE_HEX_RAW:
                    size = mapping.hexValue.length() / 2;
                    break;
            }

            // Attempt to restore original values
            if (hasOriginalValues(mapping.address, size)) {
                bool success = restoreOriginalValues(mapping.address);
                if (success) {
                    restoredCount++;
                } else {
                    failedCount++;
                }
            }
        }

        // Log restoration results
        if (Debug::enableConsole && (restoredCount > 0 || failedCount > 0)) {
            std::cout << "[DynamicQuickPatch - Memory Reset]" << std::endl;
            std::cout << "Restored " << restoredCount << " memory locations" << std::endl;
            if (failedCount > 0) {
                std::cout << "Failed to restore " << failedCount << " memory locations" << std::endl;
            }
            std::cout << std::endl;
        }

        // Clear stored memory values
        originalMemoryValues.clear();
    }

    /**
     * @brief Handles game loading.
     * @param id Save slot ID.
     * @param data Pointer to save data.
     * @param length Length of save data.
     * @details Sets a flag to trigger patch updates when returning to map.
     */
    void onLoadGame(int id, char* data, int length) {
        // Set flag to trigger patch updates
        gameJustLoaded = true;
    }

    /**
     * @brief Plugin shutdown handler.
     * @details Called when the plugin is being unloaded. Cleans up console
     *          resources if debug output was enabled.
     */
    void onExit() {
        // Cleanup console resources
        if (Debug::enableConsole) {
            Debug::cleanupConsole();
        }
    }

    /**
     * @brief Frame update handler.
     * @param scene Current game scene.
     * @details Updates memory patches when returning to map after loading.
     *          Only applies patches configured with OnLoadGame=true.
     */
    void onFrame(RPG::Scene scene) {
        // Check for map return after game load
        if (gameJustLoaded && scene == RPG::SCENE_MAP) {
            if (Debug::enableConsole) {
                std::cout << "[DynamicQuickPatch - Load Game]" << std::endl;
                std::cout << "Returned to map after loading. Updating memory patches..." << std::endl;
                std::cout << std::endl;
            }

            // Update patches configured for load game
            const auto& mappings = DynamicQuickPatchConfig::getMappings();
            int appliedCount = 0;
            int skippedCount = 0;

            for (const auto& mapping : mappings) {
                if (mapping.applyOnLoadGame) {
                    // Validate variable ID range
                    if (mapping.variableId > 0 && mapping.variableId <= DynamicQuickPatchConfig::getMaxVariableId()) {
                        int value = RPG::variables[mapping.variableId];
                        updateQuickPatch(mapping, value);
                        appliedCount++;
                    } else {
                        if (Debug::enableConsole) {
                            std::cout << "[DynamicQuickPatch - Load Game Error]" << std::endl;
                            std::cout << "Invalid variable ID: " << mapping.variableId << std::endl;
                            std::cout << std::endl;
                        }
                    }
                } else {
                    skippedCount++;
                }
            }

            // Log patch update summary
            if (Debug::enableConsole && (appliedCount > 0 || skippedCount > 0)) {
                std::cout << "[DynamicQuickPatch - Load Game Summary]" << std::endl;
                std::cout << "Applied " << appliedCount << " patches on load game." << std::endl;
                std::cout << "Skipped " << skippedCount << " patches (OnLoadGame=false)." << std::endl;
                std::cout << std::endl;
            }

            // Reset load game flag
            gameJustLoaded = false;
        }
    }

    /**
     * @brief Handles variable changes.
     * @param id ID of the variable being changed.
     * @param value New value of the variable.
     * @return Always returns true to continue normal processing.
     * @details Checks if the changed variable has quickpatch mappings and
     *          updates ALL memory locations mapped to this variable.
     *          Multiple patches can share the same variable ID.
     */
    bool onSetVariable(int id, int value) {
        // Find ALL mappings for this variable ID
        const auto& mappings = DynamicQuickPatchConfig::getMappings();
        int updatedCount = 0;

        // Iterate through all mappings to find matches
        for (const auto& mapping : mappings) {
            if (mapping.variableId == id) {
                updateQuickPatch(mapping, value);
                updatedCount++;
            }
        }

        // Optional: Log if multiple patches were updated
        if (Debug::enableConsole && updatedCount > 1) {
            std::cout << "[DynamicQuickPatch - Multi-Patch Update]" << std::endl;
            std::cout << "Updated " << updatedCount << " patches for variable " << id << std::endl;
            std::cout << "New value: " << value << std::endl;
            std::cout << std::endl;
        }

        return true;
    }
} // namespace DynamicQuickPatch
