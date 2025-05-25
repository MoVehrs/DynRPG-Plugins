/*
 * Main implementation file for the BareHanded plugin.
 * Contains the primary BareHanded namespace with all callback implementations.
 *
 * This plugin allows actors without equipped weapons to automatically equip a
 * configurable "unarmed" weapon, giving them proper stats for barehanded combat.
 */

// Include our modular code
#include "dialog.cpp"
#include "bare_handed_config.cpp"

// Main namespace to handle callbacks
namespace BareHanded
{
    // Global state variables
    bool weaponsEquipped = false;

    // Helper function to find actor by variable ID
    int findActorByVariableId(int variableId) {
        for (auto const& pair : BareHandedConfig::actorVariableMap) {
            if (pair.second == variableId) {
                return pair.first; // Return the actor ID
            }
        }
        return 0; // Not found
    }

    /**
     * @brief Handles plugin initialization
     *
     * @param pluginName Name of the plugin section in DynRPG.ini
     * @return bool True if initialization succeeded
     */
    bool onStartup(char *pluginName) {
        // Load configuration from DynRPG.ini
        bool result = BareHandedConfig::LoadConfig(pluginName);

        // Display debug message if enabled
        if (BareHandedConfig::enableDebugConfig) {
            std::stringstream ss;
            ss << "BareHanded Plugin Initialized\n";
            ss << "Configured actors with fixed weapon IDs: " << BareHandedConfig::actorWeaponMap.size() << "\n";
            ss << "Configured actors with variable-based weapon IDs: " << BareHandedConfig::actorVariableMap.size() << "\n";
            ss << "Total configured actors: " << (BareHandedConfig::actorWeaponMap.size() + BareHandedConfig::actorVariableMap.size()) << "\n";

            Dialog::Show(ss.str(), "BareHanded - Configuration");
        }

        return result;
    }

    /**
     * @brief Called on each frame to manage weapon equipping/unequipping
     *
     * @param scene Current game scene
     */
    void onFrame(RPG::Scene scene) {
        // Map scene - equip bare hand weapons
        if (scene == RPG::SCENE_MAP) {
            if (!weaponsEquipped) {
                // For each party member (max 4 in RPG Maker 2003)
                for (int i = 0; i < 4; ++i) {
                    RPG::Actor* actor = RPG::Actor::partyMember(i);

                    // Check if actor exists and has no weapons in either slot
                    // For dual-wielding actors, we need to check both weapon and shield slots
                    bool hasNoWeapons = (actor && actor->weaponId == 0);

                    // If actor can dual-wield, also check the shield slot
                    if (hasNoWeapons && actor->twoWeapons && actor->shieldId > 0) {
                        hasNoWeapons = false; // Actor has a weapon in shield slot
                    }

                    if (hasNoWeapons) {
                        // Check if we have a configuration for this actor
                        int actorId = actor->id;
                        if (BareHandedConfig::actorWeaponMap.find(actorId) != BareHandedConfig::actorWeaponMap.end()) {
                            // Only equip if the weapon ID is positive
                            int weaponId = BareHandedConfig::actorWeaponMap[actorId];
                            if (weaponId > 0) {
                                // Equip the configured bare hand weapon
                                actor->weaponId = weaponId;

                                if (BareHandedConfig::enableDebugRuntime) {
                                    std::stringstream ss;
                                    ss << "Equipped actor " << actorId << " with fixed bare hand weapon ID " << weaponId;
                                    Dialog::Show(ss.str(), "BareHanded - Runtime Debug");
                                }
                            }
                        }
                    }
                }
                weaponsEquipped = true;
            }

            // Handle variable-based weapon IDs on the map
            for (int i = 0; i < 4; ++i) {
                RPG::Actor* actor = RPG::Actor::partyMember(i);
                if (actor) {
                    int actorId = actor->id;

                    // For actors with variable-based weapon IDs
                    if (BareHandedConfig::actorVariableMap.find(actorId) != BareHandedConfig::actorVariableMap.end()) {

                        int variableId = BareHandedConfig::actorVariableMap[actorId];
                        int weaponId = RPG::variables[variableId];

                        // Check if actor has no weapons in either slot and the variable contains a valid weapon ID
                        bool hasNoWeapons = (actor->weaponId == 0);

                        // If actor can dual-wield, also check the shield slot
                        if (hasNoWeapons && actor->twoWeapons && actor->shieldId > 0) {
                            hasNoWeapons = false; // Actor has a weapon in shield slot
                        }

                        if (hasNoWeapons && weaponId > 0) {
                            // Equip the weapon ID from the variable
                            actor->weaponId = weaponId;

                            if (BareHandedConfig::enableDebugRuntime) {
                                std::stringstream ss;
                                ss << "Equipped actor " << actorId << " with variable-based weapon ID " << weaponId
                                   << " from variable ID " << variableId;
                                Dialog::Show(ss.str(), "BareHanded - Runtime Debug");
                            }
                        }
                    }
                }
            }
        }
        // Menu or Shop scene - remove bare hand weapons to allow normal equipment changes
        else if (scene == RPG::SCENE_MENU || scene == RPG::SCENE_SHOP) {
            if (weaponsEquipped) {
                // For each party member (max 4 in RPG Maker 2003)
                for (int i = 0; i < 4; ++i) {
                    RPG::Actor* actor = RPG::Actor::partyMember(i);
                    if (actor) {
                        int actorId = actor->id;

                        // First check fixed bare hand weapon IDs
                        if (BareHandedConfig::actorWeaponMap.find(actorId) != BareHandedConfig::actorWeaponMap.end()) {
                            int weaponId = BareHandedConfig::actorWeaponMap[actorId];
                            // Only process if it's a positive weapon ID and it's equipped
                            if (weaponId > 0 && actor->weaponId == weaponId) {
                                // Unequip the bare hand weapon
                                actor->weaponId = 0;

                                if (BareHandedConfig::enableDebugRuntime) {
                                    std::stringstream ss;
                                    ss << "Unequipped fixed bare hand weapon ID " << weaponId << " from actor " << actorId;
                                    Dialog::Show(ss.str(), "BareHanded - Runtime Debug");
                                }
                            }
                        }

                        // Then check variable-based weapon IDs
                        if (BareHandedConfig::actorVariableMap.find(actorId) != BareHandedConfig::actorVariableMap.end()) {
                            int variableId = BareHandedConfig::actorVariableMap[actorId];
                            int weaponId = RPG::variables[variableId];

                            // Only process if it's a positive weapon ID and it's equipped
                            if (weaponId > 0 && actor->weaponId == weaponId) {
                                // Unequip the variable-based bare hand weapon
                                actor->weaponId = 0;

                                if (BareHandedConfig::enableDebugRuntime) {
                                    std::stringstream ss;
                                    ss << "Unequipped variable-based weapon ID " << weaponId
                                       << " from variable ID " << variableId << " from actor " << actorId;
                                    Dialog::Show(ss.str(), "BareHanded - Runtime Debug");
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
     * @brief Called when a variable is changed in-game
     *
     * @param id Variable ID being changed
     * @param value New value of the variable
     * @return bool True to allow the change, false to prevent it
     */
    bool onSetVariable(int id, int value) {
        // Check if this variable is mapped to any actor
        int actorId = findActorByVariableId(id);

        if (actorId > 0) {
            // If the variable is mapped to an actor and the value is <= 0, unequip any weapon
            if (value <= 0) {
                // Check if actor is in the party
                for (int i = 0; i < 4; ++i) {
                    RPG::Actor* actor = RPG::Actor::partyMember(i);
                    if (actor && actor->id == actorId) {
                        // Get the current weapon ID (for debug purposes)
                        int currentWeaponId = actor->weaponId;

                        // Unequip the weapon (if it's currently equipped)
                        if (currentWeaponId > 0) {
                            actor->weaponId = 0;

                            if (BareHandedConfig::enableDebugRuntime) {
                                std::stringstream ss;
                                ss << "Variable ID " << id << " set to " << value
                                   << ". Unequipped variable-based weapon ID " << currentWeaponId
                                   << " from variable ID " << id << " from actor " << actorId;
                                Dialog::Show(ss.str(), "BareHanded - Runtime Debug");
                            }
                        }
                        break; // Found the actor, exit the loop
                    }
                }
            }
        }

        // Always allow the variable change
        return true;
    }

    /**
     * @brief Called when a comment command is encountered in an event
     *
     * @param text Raw comment text
     * @param parsedData Parsed comment data
     * @param nextScriptLine Next script line
     * @param scriptData Script data
     * @param eventId Event ID
     * @param pageId Page ID
     * @param lineId Line ID
     * @param nextLineId Next line ID
     * @return bool False if command was handled, true to let other plugins handle it
     */
    bool onComment(const char *text, const RPG::ParsedCommentData *parsedData,
                  RPG::EventScriptLine *nextScriptLine, RPG::EventScriptData *scriptData,
                  int eventId, int pageId, int lineId, int *nextLineId) {

        // Check if this is a plugin command (starts with @)
        if (parsedData->command[0] == '\0') {
            // Not a command
            return true;
        }

        // Convert command to lowercase for case-insensitive comparison
        std::string cmd = parsedData->command;
        for (size_t i = 0; i < cmd.length(); i++) {
            cmd[i] = tolower(cmd[i]);
        }

        // Command 1: Unequip bare hand weapon
        if (cmd == "unequipbarehand") {
            // Expect actor ID as parameter
            if (parsedData->parametersCount < 1) {
                if (BareHandedConfig::enableDebugRuntime) {
                    Dialog::Show("Missing actor ID parameter", "BareHanded - Runtime Error");
                }
                return false;
            }

            // Get actor ID from parameter - DynRPG already handles V10 syntax
            // and gives us the final number value
            int actorId = parsedData->parameters[0].number;

            if (actorId <= 0) {
                if (BareHandedConfig::enableDebugRuntime) {
                    std::stringstream ss;
                    ss << "Invalid actor ID " << actorId << " (must be > 0)";
                    Dialog::Show(ss.str(), "BareHanded - Runtime Error");
                }
                return false;
            }

            // Find the actor in the party
            RPG::Actor* actor = nullptr;
            for (int i = 0; i < 4; ++i) {
                RPG::Actor* partyMember = RPG::Actor::partyMember(i);
                if (partyMember && partyMember->id == actorId) {
                    actor = partyMember;
                    break;
                }
            }

            if (!actor) {
                if (BareHandedConfig::enableDebugRuntime) {
                    std::stringstream ss;
                    ss << "Actor " << actorId << " not found in party";
                    Dialog::Show(ss.str(), "BareHanded - Runtime Error");
                }
                return false;
            }

            // Check both weapon slots for dual-wielding actors
            bool weaponUnequipped = false;
            bool shieldUnequipped = false;

            // Primary weapon slot check
            int currentWeaponId = actor->weaponId;
            if (currentWeaponId > 0) {
                bool isPrimaryBareHandWeapon = false;

                // Check fixed weapon map for primary slot
                if (BareHandedConfig::actorWeaponMap.find(actorId) != BareHandedConfig::actorWeaponMap.end()) {
                    int bareHandWeaponId = BareHandedConfig::actorWeaponMap[actorId];
                    if (currentWeaponId == bareHandWeaponId) {
                        isPrimaryBareHandWeapon = true;
                    }
                }

                // Check variable-based weapon map for primary slot
                if (!isPrimaryBareHandWeapon && BareHandedConfig::actorVariableMap.find(actorId) != BareHandedConfig::actorVariableMap.end()) {
                    int variableId = BareHandedConfig::actorVariableMap[actorId];
                    int variableWeaponId = RPG::variables[variableId];

                    // Only consider if variable contains a positive value and matches current weapon
                    if (variableWeaponId > 0 && currentWeaponId == variableWeaponId) {
                        isPrimaryBareHandWeapon = true;
                    }
                }

                // Unequip primary weapon if it's a bare hand weapon
                if (isPrimaryBareHandWeapon) {
                    actor->weaponId = 0;
                    weaponUnequipped = true;

                    if (BareHandedConfig::enableDebugRuntime) {
                        std::stringstream ss;
                        ss << "Unequipped bare hand weapon ID " << currentWeaponId << " from actor " << actorId << "'s primary slot";
                        Dialog::Show(ss.str(), "BareHanded - Runtime Debug");
                    }
                } else if (BareHandedConfig::enableDebugRuntime) {
                    std::stringstream ss;
                    ss << "Weapon ID " << currentWeaponId << " in actor " << actorId << "'s primary slot is not a bare hand weapon";
                    Dialog::Show(ss.str(), "BareHanded - Runtime Debug");
                }
            } else if (BareHandedConfig::enableDebugRuntime) {
                std::stringstream ss;
                ss << "No weapon equipped in actor " << actorId << "'s primary slot";
                Dialog::Show(ss.str(), "BareHanded - Runtime Debug");
            }

            // Always check secondary weapon slot (shield) for all actors
            // This ensures we always show messages for both slots
            int currentShieldId = actor->shieldId;
            if (currentShieldId > 0) {
                bool isSecondaryBareHandWeapon = false;

                // Check fixed weapon map for secondary slot
                if (BareHandedConfig::actorWeaponMap.find(actorId) != BareHandedConfig::actorWeaponMap.end()) {
                    int bareHandWeaponId = BareHandedConfig::actorWeaponMap[actorId];
                    if (currentShieldId == bareHandWeaponId) {
                        isSecondaryBareHandWeapon = true;
                    }
                }

                // Check variable-based weapon map for secondary slot
                if (!isSecondaryBareHandWeapon && BareHandedConfig::actorVariableMap.find(actorId) != BareHandedConfig::actorVariableMap.end()) {
                    int variableId = BareHandedConfig::actorVariableMap[actorId];
                    int variableWeaponId = RPG::variables[variableId];

                    // Only consider if variable contains a positive value and matches current weapon
                    if (variableWeaponId > 0 && currentShieldId == variableWeaponId) {
                        isSecondaryBareHandWeapon = true;
                    }
                }

                // Unequip secondary weapon if it's a bare hand weapon
                if (isSecondaryBareHandWeapon) {
                    actor->shieldId = 0;
                    shieldUnequipped = true;

                    if (BareHandedConfig::enableDebugRuntime) {
                        std::stringstream ss;
                        ss << "Unequipped bare hand weapon ID " << currentShieldId << " from actor " << actorId << "'s secondary slot";
                        Dialog::Show(ss.str(), "BareHanded - Runtime Debug");
                    }
                } else if (BareHandedConfig::enableDebugRuntime) {
                    std::stringstream ss;
                    ss << "Weapon ID " << currentShieldId << " in actor " << actorId << "'s secondary slot is not a bare hand weapon";
                    Dialog::Show(ss.str(), "BareHanded - Runtime Debug");
                }
            } else if (BareHandedConfig::enableDebugRuntime && actor->twoWeapons) {
                std::stringstream ss;
                ss << "No weapon equipped in actor " << actorId << "'s secondary slot";
                Dialog::Show(ss.str(), "BareHanded - Runtime Debug");
            }

            return false;
        }

        // Command 2: Update equipment (trigger bare hand equipping)
        if (cmd == "updatebarehand") {
            // Reset the weaponsEquipped flag to force re-equipping
            weaponsEquipped = false;

            // For each party member (max 4 in RPG Maker 2003)
            for (int i = 0; i < 4; ++i) {
                RPG::Actor* actor = RPG::Actor::partyMember(i);

                // Check if actor exists and has no weapons in either slot
                // For dual-wielding actors, we need to check both weapon and shield slots
                bool hasNoWeapons = (actor && actor->weaponId == 0);

                // If actor can dual-wield, also check the shield slot
                if (hasNoWeapons && actor->twoWeapons && actor->shieldId > 0) {
                    hasNoWeapons = false; // Actor has a weapon in shield slot
                }

                if (hasNoWeapons) {
                    // Check if we have a configuration for this actor
                    int actorId = actor->id;

                    // Check fixed weapon configurations
                    if (BareHandedConfig::actorWeaponMap.find(actorId) != BareHandedConfig::actorWeaponMap.end()) {
                        int weaponId = BareHandedConfig::actorWeaponMap[actorId];
                        if (weaponId > 0) {
                            // Equip the configured bare hand weapon
                            actor->weaponId = weaponId;

                            if (BareHandedConfig::enableDebugRuntime) {
                                std::stringstream ss;
                                ss << "Equipped actor " << actorId << " with fixed bare hand weapon ID " << weaponId;
                                Dialog::Show(ss.str(), "BareHanded - Runtime Debug");
                            }
                        }
                    }

                    // Check variable-based weapon configurations
                    if (BareHandedConfig::actorVariableMap.find(actorId) != BareHandedConfig::actorVariableMap.end()) {
                        int variableId = BareHandedConfig::actorVariableMap[actorId];
                        int weaponId = RPG::variables[variableId];

                        // Check if actor has no weapon and the variable contains a valid weapon ID
                        if (actor->weaponId == 0 && weaponId > 0) {
                            // Equip the weapon ID from the variable
                            actor->weaponId = weaponId;

                            if (BareHandedConfig::enableDebugRuntime) {
                                std::stringstream ss;
                                ss << "Equipped actor " << actorId << " with variable-based weapon ID " << weaponId
                                   << " from variable ID " << variableId;
                                Dialog::Show(ss.str(), "BareHanded - Runtime Debug");
                            }
                        }
                    }
                }
            }
            weaponsEquipped = true;

            return false;
        }

        // Not a command we recognize
        return true;
    }
}
