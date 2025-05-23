/*
 * Core implementation for the DynamicQuickPatch plugin.
 * Contains all the memory patching functionality.
 */

// Include our modular code
#include "dialog.cpp"
#include "dynamic_quickpatch_config.cpp"

namespace DynamicQuickPatch
{
    // Flag to track when a game is loaded
    static bool gameJustLoaded = false;

    // Constants for data type ranges
    const int DQP_INT8_MIN = -127;
    const int DQP_INT8_MAX = 127;
    const int DQP_INT32_MIN = std::numeric_limits<int>::min();
    const int DQP_INT32_MAX = std::numeric_limits<int>::max();

    // Map to store original memory values before patching
    // Key: memory address, Value: vector of original bytes
    static std::map<unsigned int, std::vector<unsigned char>> originalMemoryValues;

    /**
     * @brief Helper function to check if we have stored original memory values
     *
     * @param address Memory address to check
     * @param length Number of bytes to check
     * @return bool True if we have stored values for this address range
     */
    bool hasOriginalValues(unsigned int address, size_t length) {
        return originalMemoryValues.find(address) != originalMemoryValues.end() &&
               originalMemoryValues[address].size() >= length;
    }

    /**
     * @brief Helper function to store original memory values
     *
     * @param address Memory address to store
     * @param length Number of bytes to store
     */
    void storeOriginalValues(unsigned int address, size_t length) {
        // Skip if we already have this address stored
        if (hasOriginalValues(address, length)) {
            return;
        }

        // Add safety check for valid memory address
        if (address == 0 || address >= 0xFFFFFFFF - length) {
            if (DynamicQuickPatchConfig::isDebugEnabled()) {
                std::stringstream ss;
                ss << "Invalid memory address: 0x" << std::hex << std::uppercase << address;
                Dialog::Show(ss.str(), "DynamicQuickPatch - Error");
            }
            return;
        }

        // Use standard C++ exception handling to catch access violations
        try {
            // Store the original values
            std::vector<unsigned char> originalBytes;
            for (size_t i = 0; i < length; i++) {
                originalBytes.push_back(*((unsigned char*)(address + i)));
            }
            originalMemoryValues[address] = originalBytes;

            if (DynamicQuickPatchConfig::isDebugEnabled()) {
                std::stringstream ss;
                ss << "Stored original memory values at 0x" << std::hex << std::uppercase << address
                   << " (" << std::dec << length << " bytes)";
                Dialog::Show(ss.str(), "DynamicQuickPatch - Original Values Stored");
            }
        } catch (...) {
            if (DynamicQuickPatchConfig::isDebugEnabled()) {
                std::stringstream ss;
                ss << "Memory access violation at 0x" << std::hex << std::uppercase << address;
                Dialog::Show(ss.str(), "DynamicQuickPatch - Error");
            }
            return;
        }

        // Debug message is now inside the try block
    }

    /**
     * @brief Helper function to restore original memory values
     *
     * @param address Memory address to restore
     * @return bool True if the restore was successful
     */
    bool restoreOriginalValues(unsigned int address) {
        if (originalMemoryValues.find(address) == originalMemoryValues.end()) {
            return false;
        }

        // Add safety check for valid memory address
        if (address == 0 || address >= 0xFFFFFFFF - originalMemoryValues[address].size()) {
            if (DynamicQuickPatchConfig::isDebugEnabled()) {
                std::stringstream ss;
                ss << "Invalid memory address for restore: 0x" << std::hex << std::uppercase << address;
                Dialog::Show(ss.str(), "DynamicQuickPatch - Error");
            }
            return false;
        }

        bool success = false;
        try {
            const auto& originalBytes = originalMemoryValues[address];
            for (size_t i = 0; i < originalBytes.size(); i++) {
                *((unsigned char*)(address + i)) = originalBytes[i];
            }
            success = true;
        } catch (...) {
            if (DynamicQuickPatchConfig::isDebugEnabled()) {
                std::stringstream ss;
                ss << "Memory access violation during restore at 0x" << std::hex << std::uppercase << address;
                Dialog::Show(ss.str(), "DynamicQuickPatch - Error");
            }
            return false;
        }

        return success;
    }

    /**
     * @brief Helper function to write an 8-bit signed integer (% notation)
     *
     * @param address Memory address to write to
     * @param value The value to write
     */
    void write8bitValue(unsigned int address, int value) {
        // Value range is already validated in updateQuickPatch

        // Add safety check for valid memory address
        if (address == 0 || address >= 0xFFFFFFFF) {
            if (DynamicQuickPatchConfig::isDebugEnabled()) {
                std::stringstream ss;
                ss << "Invalid memory address for 8-bit write: 0x" << std::hex << std::uppercase << address;
                Dialog::Show(ss.str(), "DynamicQuickPatch - Error");
            }
            return;
        }

        // Store original value if we don't have it yet
        if (!hasOriginalValues(address, 1)) {
            storeOriginalValues(address, 1);
        }

        try {
            *((char*)address) = (char)value;
        } catch (...) {
            if (DynamicQuickPatchConfig::isDebugEnabled()) {
                std::stringstream ss;
                ss << "Memory access violation during 8-bit write at 0x" << std::hex << std::uppercase << address;
                Dialog::Show(ss.str(), "DynamicQuickPatch - Error");
            }
        }
    }

    /**
     * @brief Helper function to write a 32-bit signed integer (# notation)
     *
     * @param address Memory address to write to
     * @param value The value to write
     */
    void write32bitValue(unsigned int address, int value) {
        // Value range is already validated in updateQuickPatch

        // Add safety check for valid memory address
        if (address == 0 || address >= 0xFFFFFFFF - sizeof(int)) {
            if (DynamicQuickPatchConfig::isDebugEnabled()) {
                std::stringstream ss;
                ss << "Invalid memory address for 32-bit write: 0x" << std::hex << std::uppercase << address;
                Dialog::Show(ss.str(), "DynamicQuickPatch - Error");
            }
            return;
        }

        // Store original value if we don't have it yet
        if (!hasOriginalValues(address, sizeof(int))) {
            storeOriginalValues(address, sizeof(int));
        }

        try {
            *((int*)address) = value;
        } catch (...) {
            if (DynamicQuickPatchConfig::isDebugEnabled()) {
                std::stringstream ss;
                ss << "Memory access violation during 32-bit write at 0x" << std::hex << std::uppercase << address;
                Dialog::Show(ss.str(), "DynamicQuickPatch - Error");
            }
        }
    }

    /**
     * @brief Helper function to write raw hex values
     *
     * @param address Memory address to write to
     * @param hexString Hexadecimal string to convert and write
     */
    void writeHexValue(unsigned int address, const std::string& hexString) {
        // Store original values if we don't have them yet
        size_t byteCount = hexString.length() / 2;
        if (!hasOriginalValues(address, byteCount)) {
            storeOriginalValues(address, byteCount);
        }

        // Add safety check for valid memory address
        if (address == 0 || address >= 0xFFFFFFFF - byteCount) {
            if (DynamicQuickPatchConfig::isDebugEnabled()) {
                std::stringstream ss;
                ss << "Invalid memory address for hex write: 0x" << std::hex << std::uppercase << address;
                Dialog::Show(ss.str(), "DynamicQuickPatch - Error");
            }
            return;
        }

        try {
            // Process hex string two characters at a time
            for (size_t i = 0; i < hexString.length(); i += 2) {
                if (i + 1 >= hexString.length()) break; // Ensure we have a complete byte

                // Convert two hex characters to one byte
                std::string byteStr = hexString.substr(i, 2);
                unsigned char byte = (unsigned char)strtol(byteStr.c_str(), NULL, 16);

                // Write the byte to memory
                *((unsigned char*)(address + (i/2))) = byte;
            }
        } catch (...) {
            if (DynamicQuickPatchConfig::isDebugEnabled()) {
                std::stringstream ss;
                ss << "Memory access violation during hex write at 0x" << std::hex << std::uppercase << address;
                Dialog::Show(ss.str(), "DynamicQuickPatch - Error");
            }
        }
    }

    /**
     * @brief Validates if a value is within the valid range for a specific type
     *
     * @param value The value to validate
     * @param type The type to validate against
     * @param variableId The variable ID (for error messages)
     * @param address The memory address (for error messages)
     * @return bool True if valid, false if out of range
     */
    bool validateValue(int value, DynamicQuickPatchConfig::QuickPatchType type,
                       int variableId, unsigned int address) {
        bool isValid = true;
        std::string errorMsg;

        switch (type) {
            case DynamicQuickPatchConfig::QPTYPE_8BIT:
                if (value < DQP_INT8_MIN || value > DQP_INT8_MAX) {
                    isValid = false;
                    std::stringstream ss;
                    ss << "Value " << value << " is out of range for 8-bit type!\n"
                       << "Variable: " << variableId << "\n"
                       << "Address: 0x" << std::hex << std::uppercase << address << "\n"
                       << "Valid range: " << std::dec << DQP_INT8_MIN << " to " << DQP_INT8_MAX << "\n"
                       << "Value will be clamped to fit in range.";
                    errorMsg = ss.str();
                }
                break;

            case DynamicQuickPatchConfig::QPTYPE_32BIT:
                // This is just a sanity check - C++ int should already be 32-bit
                // It would be extremely rare for a value to be out of this range
                if (value < DQP_INT32_MIN || value > DQP_INT32_MAX) {
                    isValid = false;
                    std::stringstream ss;
                    ss << "Value " << value << " is out of range for 32-bit type!\n"
                       << "Variable: " << variableId << "\n"
                       << "Address: 0x" << std::hex << std::uppercase << address << "\n"
                       << "Value may cause undefined behavior.";
                    errorMsg = ss.str();
                }
                break;

            case DynamicQuickPatchConfig::QPTYPE_HEX_RAW:
                // No additional validation needed for hex values
                break;
        }

        // Show error message if validation failed and debug is enabled
        if (!isValid && DynamicQuickPatchConfig::isDebugEnabled()) {
            Dialog::Show(errorMsg, "DynamicQuickPatch - Range Error");
        }

        return isValid;
    }

    /**
     * @brief Updates a quickpatch mapping with a new value
     *
     * @param mapping The mapping definition to update
     * @param value The new value to apply
     */
    void updateQuickPatch(const DynamicQuickPatchConfig::QuickPatchMapping& mapping, int value) {
        // If value is 0, restore the original memory values for any patch type
        if (value == 0) {
            // Determine size based on type
            size_t size = 0;
            std::string typeStr;

            switch (mapping.type) {
                case DynamicQuickPatchConfig::QPTYPE_8BIT:
                    size = 1; // 1 byte for 8-bit values
                    typeStr = "8-bit (%)";
                    break;

                case DynamicQuickPatchConfig::QPTYPE_32BIT:
                    size = sizeof(int); // 4 bytes for 32-bit values
                    typeStr = "32-bit (#)";
                    break;

                case DynamicQuickPatchConfig::QPTYPE_HEX_RAW:
                    size = mapping.hexValue.length() / 2; // Each hex pair is one byte
                    typeStr = "Raw Hex";
                    break;
            }

            // Check if we have original values to restore
            bool restored = false;

            if (hasOriginalValues(mapping.address, size)) {
                restored = restoreOriginalValues(mapping.address);
            }

            if (DynamicQuickPatchConfig::isDebugEnabled()) {
                std::stringstream addressStream;
                addressStream << "0x" << std::hex << std::uppercase << mapping.address;

                std::string message;
                if (restored) {
                    message = "QuickPatch Disabled\n"
                              "Variable: " + std::to_string(mapping.variableId) + "\n"
                              "Address: " + addressStream.str() + "\n"
                              "Type: " + typeStr + "\n"
                              "Original memory values restored (variable = 0)";
                } else {
                    message = "QuickPatch Disabled\n"
                              "Variable: " + std::to_string(mapping.variableId) + "\n"
                              "Address: " + addressStream.str() + "\n"
                              "Type: " + typeStr + "\n"
                              "Patch is now inactive (variable = 0)\n"
                              "Note: Original values were not available to restore";
                }

                Dialog::Show(message, "DynamicQuickPatch - Patch Disabled");
            }
            return;
        }

        // Determine the old value from the memory address
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

        // Validate the value and apply appropriate range limits
        // We call validateValue for its side effects (showing warning messages) but don't need the return value
        validateValue(value, mapping.type, mapping.variableId, mapping.address);
        int adjustedValue = value;

        // Apply new value with range clamping if necessary
        switch (mapping.type) {
            case DynamicQuickPatchConfig::QPTYPE_8BIT:
                // Clamp the value to valid range
                if (adjustedValue < DQP_INT8_MIN) adjustedValue = DQP_INT8_MIN;
                if (adjustedValue > DQP_INT8_MAX) adjustedValue = DQP_INT8_MAX;
                write8bitValue(mapping.address, adjustedValue);
                break;

            case DynamicQuickPatchConfig::QPTYPE_32BIT:
                // Normally we won't need to clamp 32-bit values
                write32bitValue(mapping.address, adjustedValue);
                break;

            case DynamicQuickPatchConfig::QPTYPE_HEX_RAW:
                // For hex type, we know value > 0 at this point
                writeHexValue(mapping.address, mapping.hexValue);
                break;
        }

        // Only display debug message if enabled
        if (DynamicQuickPatchConfig::isDebugEnabled()) {
            // Prepare display values
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

            // Format the address as hexadecimal
            std::stringstream addressStream;
            addressStream << "0x" << std::hex << std::uppercase << mapping.address;

            // Build message
            std::string message = "QuickPatch Updated\n"
                                "Variable: " + std::to_string(mapping.variableId) + "\n"
                                "Address: " + addressStream.str() + "\n"
                                "Type: " + typeStr + "\n"
                                "Old Value: " + oldValueStr + "\n"
                                "New Value: " + newValueStr + rangeInfo;

            Dialog::Show(message, "DynamicQuickPatch - Memory Update");
        }
    }

    /**
     * @brief Handles variable changes
     *
     * @param id ID of the variable being changed
     * @param value New value of the variable
     * @return bool Always returns true to continue normal processing
     */
    bool onSetVariable(int id, int value) {
        const auto& mappings = DynamicQuickPatchConfig::getMappings();

        // Use find_if to locate the mapping with the matching variable ID
        auto it = std::find_if(mappings.begin(), mappings.end(),
            [id](const DynamicQuickPatchConfig::QuickPatchMapping& mapping) {
                return mapping.variableId == id;
            });

        if (it != mappings.end()) {
            updateQuickPatch(*it, value);
        }

        // Always return true to continue normal processing
        return true;
    }

    /**
     * @brief Handles game loading
     *
     * @param id Save slot ID
     * @param data Pointer to save data
     * @param length Length of save data
     */
    void onLoadGame(int id, char* data, int length) {
        // Set the flag to indicate a game was just loaded
        gameJustLoaded = true;
    }

    /**
     * @brief Frame update handler
     *
     * @param scene Current game scene
     */
    void onFrame(RPG::Scene scene) {
        // Check if a game was loaded and we're in the map scene
        if (gameJustLoaded && scene == RPG::SCENE_MAP) {
            if (DynamicQuickPatchConfig::isDebugEnabled()) {
                Dialog::Show("Returned to map after loading. Updating memory patches...",
                               "DynamicQuickPatch - Updating Memory");
            }

            // Update variable mappings that are configured to apply on load game
            const auto& mappings = DynamicQuickPatchConfig::getMappings();
            int appliedCount = 0;
            int skippedCount = 0;

            for (const auto& mapping : mappings) {
                // Only apply patches that are configured to run on load game
                if (mapping.applyOnLoadGame) {
                    // Add safety check for variable ID bounds
                    if (mapping.variableId > 0 && mapping.variableId <= DynamicQuickPatchConfig::getMaxVariableId()) {
                        int value = RPG::variables[mapping.variableId];
                        updateQuickPatch(mapping, value);
                        appliedCount++;
                    } else {
                        if (DynamicQuickPatchConfig::isDebugEnabled()) {
                            std::stringstream ss;
                            ss << "Invalid variable ID: " << mapping.variableId;
                            Dialog::Show(ss.str(), "DynamicQuickPatch - Error");
                        }
                    }
                } else {
                    skippedCount++;
                }
            }

            // Show debug info about applied patches
            if (DynamicQuickPatchConfig::isDebugEnabled() && (appliedCount > 0 || skippedCount > 0)) {
                std::stringstream ss;
                ss << "Applied " << appliedCount << " patches on load game.\n"
                   << "Skipped " << skippedCount << " patches (OnLoadGame=false).";
                Dialog::Show(ss.str(), "DynamicQuickPatch - Load Game Patches");
            }

            // Reset the flag
            gameJustLoaded = false;
        }
    }

    /**
     * @brief Handles starting a new game or returning to the title screen
     *
     * This is called when starting a new game or when returning to the title screen
     * using F12. We need to restore all original memory values to prevent patches
     * from persisting across game sessions.
     */
    void onNewGame() {
        // Restore all original memory values for all types of patches
        int restoredCount = 0;
        int failedCount = 0;

        // Get all mappings to find addresses that need to be restored
        const auto& mappings = DynamicQuickPatchConfig::getMappings();

        // Keep track of addresses we've already processed to avoid duplicates
        std::set<unsigned int> processedAddresses;

        for (const auto& mapping : mappings) {
            // Skip if we've already processed this address
            if (processedAddresses.find(mapping.address) != processedAddresses.end()) {
                continue;
            }

            // Mark this address as processed
            processedAddresses.insert(mapping.address);

            // Determine size based on type
            size_t size = 0;
            switch (mapping.type) {
                case DynamicQuickPatchConfig::QPTYPE_8BIT:
                    size = 1; // 1 byte for 8-bit values
                    break;

                case DynamicQuickPatchConfig::QPTYPE_32BIT:
                    size = 4; // 4 bytes for 32-bit values
                    break;

                case DynamicQuickPatchConfig::QPTYPE_HEX_RAW:
                    size = mapping.hexValue.length() / 2; // Each hex pair is one byte
                    break;
            }

            // Attempt to restore original values if we have them
            if (hasOriginalValues(mapping.address, size)) {
                bool success = restoreOriginalValues(mapping.address);
                if (success) {
                    restoredCount++;
                } else {
                    failedCount++;
                }
            }
        }

        // Show debug info about restored memory values
        if (DynamicQuickPatchConfig::isDebugEnabled() && (restoredCount > 0 || failedCount > 0)) {
            std::stringstream ss;
            ss << "Restored " << restoredCount << " memory locations\n";

            if (failedCount > 0) {
                ss << "Failed to restore " << failedCount << " memory locations";
            }

            Dialog::Show(ss.str(), "DynamicQuickPatch - Memory Reset");
        }

        // Clear the originalMemoryValues map to start fresh
        originalMemoryValues.clear();
    }
} // namespace DynamicQuickPatch
