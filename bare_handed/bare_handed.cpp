/**
 * @file bare_handed.cpp
 * @brief Main implementation of the BareHanded plugin.
 * @details Contains the primary BareHanded namespace with all callback implementations
 *          and core functionality for managing automatic weapon equipping for unarmed combat.
 */

#include "bare_handed_debug.cpp"
#include "bare_handed_config.cpp"

/**
 * @namespace BareHanded
 * @brief Core implementation namespace for the BareHanded plugin.
 * @details Handles automatic weapon equipping and unequipping for unarmed combat.
 *          Provides functionality to automatically equip configurable "bare hand"
 *          weapons when actors are unarmed, and remove them in equipment menus.
 */
namespace BareHanded
{
    /** @brief Tracks whether bare hand weapons are currently equipped. */
    bool weaponsEquipped = false;

    /**
     * @brief Finds an actor ID based on its associated variable ID.
     * @param variableId The variable ID to search for.
     * @return The actor ID if found, 0 if not found.
     * @details Searches through the actor-variable mapping to find which actor
     *          is associated with a given variable ID for weapon configuration.
     * @see BareHandedConfig::actorVariableMap
     */
    int findActorByVariableId(int variableId) {
        for (auto const& pair : BareHandedConfig::actorVariableMap) {
            if (pair.second == variableId) {
                return pair.first; // Return the actor ID
            }
        }
        return 0; // Not found
    }

    /**
     * @brief Initializes the BareHanded plugin.
     * @param pluginName Name of the plugin section in DynRPG.ini.
     * @return True if initialization succeeded, false otherwise.
     * @details Performs the following initialization steps:
     *          1. Loads configuration from DynRPG.ini
     *          2. Sets up debug console if enabled
     *          3. Initializes weapon mappings for actors
     * @see BareHandedConfig::LoadConfig
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

        // Load main configuration
        bool result = BareHandedConfig::LoadConfig(pluginName);

        // Display initialization summary if debug enabled
        if (BareHandedConfig::enableDebugConfig && Debug::enableConsole) {
            std::cout << "[BareHanded - Configuration]" << std::endl;
            std::cout << "BareHanded Plugin Initialized" << std::endl;
            std::cout << "Configured actors with fixed weapon IDs: " << BareHandedConfig::actorWeaponMap.size() << std::endl;
            std::cout << "Configured actors with variable-based weapon IDs: " << BareHandedConfig::actorVariableMap.size() << std::endl;
            std::cout << "Total configured actors: " << (BareHandedConfig::actorWeaponMap.size() + BareHandedConfig::actorVariableMap.size()) << std::endl;
            std::cout << std::endl;
        }

        return result;
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
     * @brief Processes frame updates to manage weapon equipping.
     * @param scene The current game scene.
     * @details Handles automatic equipping and unequipping of bare hand weapons:
     *          - In map scenes: Equips configured weapons to unarmed actors
     *          - In menu/shop scenes: Removes bare hand weapons to allow equipment changes
     * @note Handles both fixed weapon IDs and variable-based weapon configurations.
     * @see BareHandedConfig::actorWeaponMap
     * @see BareHandedConfig::actorVariableMap
     */
    void onFrame(RPG::Scene scene) {
        // Process map scene - equip bare hand weapons
        if (scene == RPG::SCENE_MAP) {
            if (!weaponsEquipped) {
                // Process each party member (max 4 in RPG Maker 2003)
                for (int i = 0; i < 4; ++i) {
                    RPG::Actor* actor = RPG::Actor::partyMember(i);

                    // Check for empty weapon slots, considering dual-wielding
                    bool hasNoWeapons = (actor && actor->weaponId == 0);

                    // Check shield slot for dual-wielding actors
                    if (hasNoWeapons && actor->twoWeapons && actor->shieldId > 0) {
                        hasNoWeapons = false; // Actor has weapon in shield slot
                    }

                    if (hasNoWeapons) {
                        // Process fixed weapon configuration
                        int actorId = actor->id;
                        if (BareHandedConfig::actorWeaponMap.find(actorId) != BareHandedConfig::actorWeaponMap.end()) {
                            int weaponId = BareHandedConfig::actorWeaponMap[actorId];
                            if (weaponId > 0) {
                                actor->weaponId = weaponId;

                                // Log weapon equip if debug enabled
                                if (BareHandedConfig::enableDebugRuntime && Debug::enableConsole) {
                                    std::cout << "[BareHanded - Runtime Debug]" << std::endl;
                                    std::cout << "Equipped actor " << actorId << " with fixed bare hand weapon ID " << weaponId << std::endl;
                                    std::cout << std::endl;
                                }
                            }
                        }
                    }
                }
                weaponsEquipped = true;
            }

            // Process variable-based weapon configurations
            for (int i = 0; i < 4; ++i) {
                RPG::Actor* actor = RPG::Actor::partyMember(i);
                if (actor) {
                    int actorId = actor->id;

                    // Check for variable-based weapon configuration
                    if (BareHandedConfig::actorVariableMap.find(actorId) != BareHandedConfig::actorVariableMap.end()) {
                        int variableId = BareHandedConfig::actorVariableMap[actorId];
                        int weaponId = RPG::variables[variableId];

                        // Check for empty weapon slots and valid weapon ID
                        bool hasNoWeapons = (actor->weaponId == 0);

                        // Check shield slot for dual-wielding actors
                        if (hasNoWeapons && actor->twoWeapons && actor->shieldId > 0) {
                            hasNoWeapons = false; // Actor has weapon in shield slot
                        }

                        if (hasNoWeapons && weaponId > 0) {
                            actor->weaponId = weaponId;

                            // Log variable-based weapon equip if debug enabled
                            if (BareHandedConfig::enableDebugRuntime && Debug::enableConsole) {
                                std::cout << "[BareHanded - Runtime Debug]" << std::endl;
                                std::cout << "Equipped actor " << actorId << " with variable-based weapon ID " << weaponId
                                        << " from variable ID " << variableId << std::endl;
                                std::cout << std::endl;
                            }
                        }
                    }
                }
            }
        }
        // Process menu/shop scenes - remove bare hand weapons
        else if (scene == RPG::SCENE_MENU || scene == RPG::SCENE_SHOP) {
            if (weaponsEquipped) {
                // Process each party member (max 4 in RPG Maker 2003)
                for (int i = 0; i < 4; ++i) {
                    RPG::Actor* actor = RPG::Actor::partyMember(i);
                    if (actor) {
                        int actorId = actor->id;

                        // Process fixed weapon configuration
                        if (BareHandedConfig::actorWeaponMap.find(actorId) != BareHandedConfig::actorWeaponMap.end()) {
                            int weaponId = BareHandedConfig::actorWeaponMap[actorId];
                            if (weaponId > 0 && actor->weaponId == weaponId) {
                                actor->weaponId = 0;

                                // Log weapon unequip if debug enabled
                                if (BareHandedConfig::enableDebugRuntime && Debug::enableConsole) {
                                    std::cout << "[BareHanded - Runtime Debug]" << std::endl;
                                    std::cout << "Unequipped fixed bare hand weapon ID " << weaponId << " from actor " << actorId << std::endl;
                                    std::cout << std::endl;
                                }
                            }
                        }

                        // Process variable-based weapon configuration
                        if (BareHandedConfig::actorVariableMap.find(actorId) != BareHandedConfig::actorVariableMap.end()) {
                            int variableId = BareHandedConfig::actorVariableMap[actorId];
                            int weaponId = RPG::variables[variableId];

                            if (weaponId > 0 && actor->weaponId == weaponId) {
                                actor->weaponId = 0;

                                // Log variable-based weapon unequip if debug enabled
                                if (BareHandedConfig::enableDebugRuntime && Debug::enableConsole) {
                                    std::cout << "[BareHanded - Runtime Debug]" << std::endl;
                                    std::cout << "Unequipped variable-based weapon ID " << weaponId
                                            << " from variable ID " << variableId << " from actor " << actorId << std::endl;
                                    std::cout << std::endl;
                                }
                            }
                        }
                    }
                }
                weaponsEquipped = false;
            }
        }
    }

    /**
     * @brief Processes variable changes to manage weapon unequipping.
     * @param id Variable ID being changed.
     * @param value New value of the variable.
     * @return True to allow the change, false to prevent it.
     * @details Handles unequipping of variable-based bare hand weapons when their
     *          associated variable is set to 0 or negative, preventing invalid states.
     * @note Only processes variables that are mapped to actor weapon configurations.
     * @see BareHandedConfig::actorVariableMap
     */
    bool onSetVariable(int id, int value) {
        // Check for actor using this variable
        int actorId = findActorByVariableId(id);

        if (actorId > 0) {
            // Unequip weapon if variable value is invalid
            if (value <= 0) {
                // Find actor in party
                for (int i = 0; i < 4; ++i) {
                    RPG::Actor* actor = RPG::Actor::partyMember(i);
                    if (actor && actor->id == actorId) {
                        int currentWeaponId = actor->weaponId;

                        // Unequip if weapon is equipped
                        if (currentWeaponId > 0) {
                            actor->weaponId = 0;

                            // Log weapon unequip if debug enabled
                            if (BareHandedConfig::enableDebugRuntime && Debug::enableConsole) {
                                std::cout << "[BareHanded - Runtime Debug]" << std::endl;
                                std::cout << "Variable ID " << id << " set to " << value
                                        << ". Unequipped variable-based weapon ID " << currentWeaponId
                                        << " from variable ID " << id << " from actor " << actorId << std::endl;
                                std::cout << std::endl;
                            }
                        }
                        break; // Actor found, exit loop
                    }
                }
            }
        }

        return true; // Allow variable change
    }

    /**
     * @brief Processes event comments for manual weapon management.
     * @param text Raw comment text.
     * @param parsedData Parsed comment data.
     * @param nextScriptLine Next script line.
     * @param scriptData Script data.
     * @param eventId Event ID.
     * @param pageId Page ID.
     * @param lineId Line ID.
     * @param nextLineId Next line ID.
     * @return False if command was handled, true to let other plugins handle it.
     * @details Processes plugin-specific commands for manually managing bare hand weapons:
     *          - Equipping specific weapons to actors
     *          - Unequipping bare hand weapons
     *          - Modifying weapon configurations
     * @note Commands must be prefixed with "bare_handed" to be processed.
     */
    bool onComment(const char *text, const RPG::ParsedCommentData *parsedData,
                  RPG::EventScriptLine *nextScriptLine, RPG::EventScriptData *scriptData,
                  int eventId, int pageId, int lineId, int *nextLineId) {

        // Validate command format
        if (parsedData->command[0] == '\0') {
            return true; // Not a command
        }

        // Convert command to lowercase for case-insensitive comparison
        std::string cmd = parsedData->command;
        for (size_t i = 0; i < cmd.length(); i++) {
            cmd[i] = tolower(cmd[i]);
        }

        // Process unequip command
        if (cmd == "unequipbarehand") {
            // Validate actor ID parameter
            if (parsedData->parametersCount < 1) {
                if (BareHandedConfig::enableDebugRuntime && Debug::enableConsole) {
                    std::cout << "[BareHanded - Runtime Error]" << std::endl;
                    std::cout << "Missing actor ID parameter" << std::endl;
                    std::cout << std::endl;
                }
                return false;
            }

            // Get actor ID from parameter
            int actorId = parsedData->parameters[0].number;

            if (actorId <= 0) {
                if (BareHandedConfig::enableDebugRuntime && Debug::enableConsole) {
                    std::cout << "[BareHanded - Runtime Error]" << std::endl;
                    std::cout << "Invalid actor ID " << actorId << " (must be > 0)" << std::endl;
                    std::cout << std::endl;
                }
                return false;
            }

            // Find actor in party
            RPG::Actor* actor = nullptr;
            for (int i = 0; i < 4; ++i) {
                RPG::Actor* partyMember = RPG::Actor::partyMember(i);
                if (partyMember && partyMember->id == actorId) {
                    actor = partyMember;
                    break;
                }
            }

            if (!actor) {
                if (BareHandedConfig::enableDebugRuntime && Debug::enableConsole) {
                    std::cout << "[BareHanded - Runtime Error]" << std::endl;
                    std::cout << "Actor " << actorId << " not found in party" << std::endl;
                    std::cout << std::endl;
                }
                return false;
            }

            // Process weapon slots
            bool weaponUnequipped = false;
            bool shieldUnequipped = false;

            // Check primary weapon slot
            int currentWeaponId = actor->weaponId;
            if (currentWeaponId > 0) {
                bool isPrimaryBareHandWeapon = false;

                // Check fixed weapon configuration
                if (BareHandedConfig::actorWeaponMap.find(actorId) != BareHandedConfig::actorWeaponMap.end()) {
                    int bareHandWeaponId = BareHandedConfig::actorWeaponMap[actorId];
                    if (currentWeaponId == bareHandWeaponId) {
                        isPrimaryBareHandWeapon = true;
                    }
                }

                // Check variable-based weapon configuration
                if (!isPrimaryBareHandWeapon && BareHandedConfig::actorVariableMap.find(actorId) != BareHandedConfig::actorVariableMap.end()) {
                    int variableId = BareHandedConfig::actorVariableMap[actorId];
                    int variableWeaponId = RPG::variables[variableId];

                    if (variableWeaponId > 0 && currentWeaponId == variableWeaponId) {
                        isPrimaryBareHandWeapon = true;
                    }
                }

                // Unequip if bare hand weapon found
                if (isPrimaryBareHandWeapon) {
                    actor->weaponId = 0;
                    weaponUnequipped = true;

                    // Log primary weapon unequip if debug enabled
                    if (BareHandedConfig::enableDebugRuntime && Debug::enableConsole) {
                        std::cout << "[BareHanded - Runtime Debug]" << std::endl;
                        std::cout << "Unequipped bare hand weapon ID " << currentWeaponId << " from actor " << actorId << "'s primary slot" << std::endl;
                        std::cout << std::endl;
                    }
                } else if (BareHandedConfig::enableDebugRuntime && Debug::enableConsole) {
                    std::cout << "[BareHanded - Runtime Debug]" << std::endl;
                    std::cout << "Weapon ID " << currentWeaponId << " in actor " << actorId << "'s primary slot is not a bare hand weapon" << std::endl;
                    std::cout << std::endl;
                }
            } else if (BareHandedConfig::enableDebugRuntime && Debug::enableConsole) {
                std::cout << "[BareHanded - Runtime Debug]" << std::endl;
                std::cout << "No weapon equipped in actor " << actorId << "'s primary slot" << std::endl;
                std::cout << std::endl;
            }

            // Check secondary weapon slot (shield)
            int currentShieldId = actor->shieldId;
            if (currentShieldId > 0) {
                bool isSecondaryBareHandWeapon = false;

                // Check fixed weapon configuration
                if (BareHandedConfig::actorWeaponMap.find(actorId) != BareHandedConfig::actorWeaponMap.end()) {
                    int bareHandWeaponId = BareHandedConfig::actorWeaponMap[actorId];
                    if (currentShieldId == bareHandWeaponId) {
                        isSecondaryBareHandWeapon = true;
                    }
                }

                // Check variable-based weapon configuration
                if (!isSecondaryBareHandWeapon && BareHandedConfig::actorVariableMap.find(actorId) != BareHandedConfig::actorVariableMap.end()) {
                    int variableId = BareHandedConfig::actorVariableMap[actorId];
                    int variableWeaponId = RPG::variables[variableId];

                    if (variableWeaponId > 0 && currentShieldId == variableWeaponId) {
                        isSecondaryBareHandWeapon = true;
                    }
                }

                // Unequip if bare hand weapon found
                if (isSecondaryBareHandWeapon) {
                    actor->shieldId = 0;
                    shieldUnequipped = true;

                    // Log secondary weapon unequip if debug enabled
                    if (BareHandedConfig::enableDebugRuntime && Debug::enableConsole) {
                        std::cout << "[BareHanded - Runtime Debug]" << std::endl;
                        std::cout << "Unequipped bare hand weapon ID " << currentShieldId << " from actor " << actorId << "'s secondary slot" << std::endl;
                        std::cout << std::endl;
                    }
                } else if (BareHandedConfig::enableDebugRuntime && Debug::enableConsole) {
                    std::cout << "[BareHanded - Runtime Debug]" << std::endl;
                    std::cout << "Weapon ID " << currentShieldId << " in actor " << actorId << "'s secondary slot is not a bare hand weapon" << std::endl;
                    std::cout << std::endl;
                }
            } else if (BareHandedConfig::enableDebugRuntime && actor->twoWeapons && Debug::enableConsole) {
                std::cout << "[BareHanded - Runtime Debug]" << std::endl;
                std::cout << "No weapon equipped in actor " << actorId << "'s secondary slot" << std::endl;
                std::cout << std::endl;
            }

            return false;
        }

        // Process update command
        if (cmd == "updatebarehand") {
            // Reset equip state to trigger re-equipping
            weaponsEquipped = false;

            // Process each party member (max 4 in RPG Maker 2003)
            for (int i = 0; i < 4; ++i) {
                RPG::Actor* actor = RPG::Actor::partyMember(i);

                // Check for empty weapon slots, considering dual-wielding
                bool hasNoWeapons = (actor && actor->weaponId == 0);

                // Check shield slot for dual-wielding actors
                if (hasNoWeapons && actor->twoWeapons && actor->shieldId > 0) {
                    hasNoWeapons = false; // Actor has weapon in shield slot
                }

                if (hasNoWeapons) {
                    int actorId = actor->id;

                    // Process fixed weapon configuration
                    if (BareHandedConfig::actorWeaponMap.find(actorId) != BareHandedConfig::actorWeaponMap.end()) {
                        int weaponId = BareHandedConfig::actorWeaponMap[actorId];
                        if (weaponId > 0) {
                            actor->weaponId = weaponId;

                            // Log weapon equip if debug enabled
                            if (BareHandedConfig::enableDebugRuntime && Debug::enableConsole) {
                                std::cout << "[BareHanded - Runtime Debug]" << std::endl;
                                std::cout << "Equipped actor " << actorId << " with fixed bare hand weapon ID " << weaponId << std::endl;
                                std::cout << std::endl;
                            }
                        }
                    }

                    // Process variable-based weapon configuration
                    if (BareHandedConfig::actorVariableMap.find(actorId) != BareHandedConfig::actorVariableMap.end()) {
                        int variableId = BareHandedConfig::actorVariableMap[actorId];
                        int weaponId = RPG::variables[variableId];

                        // Check for empty weapon slot and valid weapon ID
                        if (actor->weaponId == 0 && weaponId > 0) {
                            actor->weaponId = weaponId;

                            // Log variable-based weapon equip if debug enabled
                            if (BareHandedConfig::enableDebugRuntime && Debug::enableConsole) {
                                std::cout << "[BareHanded - Runtime Debug]" << std::endl;
                                std::cout << "Equipped actor " << actorId << " with variable-based weapon ID " << weaponId
                                        << " from variable ID " << variableId << std::endl;
                                std::cout << std::endl;
                            }
                        }
                    }
                }
            }
            weaponsEquipped = true;

            return false;
        }

        return true; // Not a recognized command
    }
} // namespace BareHanded
