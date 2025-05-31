/**
 * @file main.cpp
 * @brief Entry point for the DynamicQuickPatch plugin for RPG Maker 2003.
 * @details Contains DynRPG callback functions that serve as entry points from the game engine
 *          into the plugin's functionality. All implementation details are organized in the
 *          DynamicQuickPatch namespace.
 */

// Core DynRPG header
#include <DynRPG/DynRPG.h>

// Standard library headers
#include <algorithm>  // For std::min, std::max
#include <fstream>    // For file operations
#include <iostream>   // For console output
#include <limits>     // For numeric limits
#include <map>        // For configuration and memory storage
#include <set>        // For storing unique indices
#include <sstream>    // For string formatting
#include <string>     // For text processing
#include <vector>     // For storing quickpatch mappings
#include <stdio.h>    // For freopen, stdout, stdin
#include <stdlib.h>   // For atoi
#include <stdint.h>   // For standard integer types
#include <windows.h>  // For console functions and memory operations

// Main implementation file - contains all namespaced code
#include "dynamic_quickpatch.cpp"

/**
 * @defgroup callbacks DynRPG Plugin Callbacks
 * @brief Global callback functions called by the DynRPG system.
 * @details These functions serve as the interface between the RPG Maker 2003 engine
 *          and the DynamicQuickPatch plugin functionality.
 */

/**
 * @brief Initializes the DynamicQuickPatch plugin.
 * @param pluginName Name of the plugin section in DynRPG.ini.
 * @return True if initialization succeeded.
 * @details Called once when the plugin is first loaded. Reads configuration from DynRPG.ini
 *          and sets up debug output if enabled.
 * @see DynamicQuickPatch::onStartup
 */
bool onStartup(char *pluginName) {
    return DynamicQuickPatch::onStartup(pluginName);
}

/**
 * @brief Processes new game or title screen return events.
 * @details Resets all memory patches to their original values when starting a new game
 *          or when returning to the title screen with F12 key.
 * @see DynamicQuickPatch::onNewGame
 */
void onNewGame() {
    DynamicQuickPatch::onNewGame();
}

/**
 * @brief Processes save game loading.
 * @param id Save slot ID.
 * @param data Pointer to save data.
 * @param length Length of save data.
 * @details Sets a flag to trigger memory patch updates when returning to the map scene.
 * @see DynamicQuickPatch::onLoadGame
 */
void onLoadGame(int id, char* data, int length) {
    DynamicQuickPatch::onLoadGame(id, data, length);
}

/**
 * @brief Performs cleanup when the plugin is unloaded.
 * @note Called when the plugin is being unloaded or the game exits.
 * @see DynamicQuickPatch::onExit
 */
void onExit() {
    DynamicQuickPatch::onExit();
}

/**
 * @brief Frame update handler.
 * @param scene Current game scene.
 * @details Updates memory patches when returning to map after loading.
 *          Only applies patches configured with OnLoadGame=true.
 * @see DynamicQuickPatch::onFrame
 */
void onFrame(RPG::Scene scene) {
    DynamicQuickPatch::onFrame(scene);
}

/**
 * @brief Processes variable changes during gameplay.
 * @param id Variable ID being changed.
 * @param value New value of the variable.
 * @return True to allow the change.
 * @details Intercepts variable changes and updates memory patches based on the
 *          configured variable-to-memory mappings.
 * @see DynamicQuickPatch::onSetVariable
 */
bool onSetVariable(int id, int value) {
    return DynamicQuickPatch::onSetVariable(id, value);
}

/** @} */ // end of callbacks group
