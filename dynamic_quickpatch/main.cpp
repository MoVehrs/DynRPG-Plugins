/*
 * Entry point for the DynamicQuickPatch plugin for RPG Maker 2000/2003.
 * This plugin maps RPG Maker 2003 variables to memory addresses,
 * allowing dynamic modification of quickpatches during gameplay.
 * 
 * All implementation details are organized in the DynamicQuickPatch namespace.
 */

// Core DynRPG header
#include <DynRPG/DynRPG.h>

// Standard library headers
#include <algorithm>  // For std::min, std::max
#include <limits>     // For numeric limits
#include <map>        // For configuration storage
#include <set>        // For storing unique indices
#include <sstream>    // For string formatting
#include <string>     // For text processing
#include <vector>     // For storing quickpatch mappings
#include <windows.h>  // For MessageBox

// Main implementation file - contains all namespaced code
#include "dynamic_quickpatch.cpp"

// ========================================================================
// DynRPG plugin entry points - global callback functions
// These are called by the DynRPG system and forward to our implementation
// ========================================================================

/**
 * @brief DynRPG callback: Plugin initialization
 * 
 * @param pluginName Name of the plugin (used to load configuration)
 * @return bool True if initialization succeeded, false if it failed
 * 
 * @note Called once when the plugin is first loaded
 *       Loads all configuration from DynRPG.ini
 */
bool onStartup(char *pluginName) {
    // Load configuration and check the result
    bool configLoaded = DynamicQuickPatchConfig::loadConfig(pluginName);
    
    // Always return true to prevent crashes
    return true;
}

/**
 * @brief DynRPG callback: Called when a variable is about to be set
 * 
 * @param id The ID of the variable being set
 * @param value The new value that will be assigned to the variable
 * @return bool Always returns true to continue normal processing
 * 
 * @note Used to intercept variable changes and update memory patches
 */
bool onSetVariable(int id, int value) {
    return DynamicQuickPatch::onSetVariable(id, value);
}

/**
 * @brief DynRPG callback: Called when a save game is loaded
 * 
 * @param id The save slot ID
 * @param data Pointer to save data
 * @param length Length of save data
 * 
 * @note Used to detect when a save game is loaded to force update quickpatches
 */
void onLoadGame(int id, char* data, int length) {
    DynamicQuickPatch::onLoadGame(id, data, length);
}

/**
 * @brief DynRPG callback: Called every frame
 * 
 * @param scene Current game scene
 * 
 * @note Used to monitor when a save is loaded and the player returns to the map
 */
void onFrame(RPG::Scene scene) {
    DynamicQuickPatch::onFrame(scene);
}

/**
 * @brief DynRPG callback: Called when starting a new game or returning to title screen
 * 
 * @note Used to reset all memory patches when starting a new game or
 *       when returning to the title screen with F12 key
 */
void onNewGame() {
    DynamicQuickPatch::onNewGame();
}
