/**
 * @file main.cpp
 * @brief Entry point for the BareHanded plugin for RPG Maker 2003.
 * @details This file contains DynRPG callback functions that serve as entry
 *          points from the game engine into the plugin's functionality.
 *          All implementation details are organized in the BareHanded namespace.
 */

// Core DynRPG header
#include <DynRPG/DynRPG.h>

// Standard library headers
#include <fstream>    // For file operations
#include <map>        // For storing configurations and state data
#include <sstream>    // For string formatting
#include <string>     // For text processing
#include <iostream>   // For console output
#include <stdio.h>    // For freopen
#include <stdlib.h>   // For atoi
#include <stdint.h>   // For standard integer types
#include <windows.h>  // For Windows API

// Main implementation file - contains all namespaced code
#include "bare_handed.cpp"

/**
 * @defgroup callbacks DynRPG Plugin Callbacks
 * @brief Global callback functions called by the DynRPG system
 * @{
 */

/**
 * @brief Initializes the BareHanded plugin.
 * @param pluginName Name of the plugin section in DynRPG.ini.
 * @return True if initialization succeeded, false if it failed.
 * @note Called once when the plugin is first loaded.
 * @see BareHanded::onStartup
 */
bool onStartup(char *pluginName) {
    return BareHanded::onStartup(pluginName);
}

/**
 * @brief Performs cleanup when the plugin is unloaded.
 * @note Called when the plugin is being unloaded or the game exits.
 * @see BareHanded::onExit
 */
void onExit() {
    BareHanded::onExit();
}

/**
 * @brief Processes frame updates during gameplay.
 * @param scene Current game scene.
 * @note Handles automatic equipping/unequipping of bare hand weapons
 *       based on the current game scene and actor state.
 * @see BareHanded::onFrame
 */
void onFrame(RPG::Scene scene) {
    BareHanded::onFrame(scene);
}

/**
 * @brief Processes variable changes during gameplay.
 * @param id Variable ID being changed.
 * @param value New value of the variable.
 * @return True to allow the change, false to prevent it.
 * @note Handles unequipping of variable-based bare hand weapons
 *       when their associated variable is set to 0 or negative.
 * @see BareHanded::onSetVariable
 */
bool onSetVariable(int id, int value) {
    return BareHanded::onSetVariable(id, value);
}

/**
 * @brief Processes comment commands during event execution.
 * @param text Raw comment text.
 * @param parsedData Parsed comment data.
 * @param nextScriptLine Next script line.
 * @param scriptData Script data.
 * @param eventId Event ID.
 * @param pageId Page ID.
 * @param lineId Line ID.
 * @param nextLineId Next line ID.
 * @return False if command was handled, true to let other plugins handle it.
 * @note Processes plugin-specific commands for manually managing bare hand weapons.
 * @see BareHanded::onComment
 */
bool onComment(const char *text, const RPG::ParsedCommentData *parsedData, 
              RPG::EventScriptLine *nextScriptLine, RPG::EventScriptData *scriptData, 
              int eventId, int pageId, int lineId, int *nextLineId) {
    return BareHanded::onComment(text, parsedData, nextScriptLine, scriptData, eventId, pageId, lineId, nextLineId);
}

/** @} */ // end of callbacks group
