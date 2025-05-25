/*
 * Entry point for the BareHanded plugin for RPG Maker 2003.
 * This file contains DynRPG callback functions that serve as entry
 * points from the game engine into our plugin's functionality.
 * 
 * All implementation details are organized in the BareHanded namespace.
 */

// Core DynRPG header
#include <DynRPG/DynRPG.h>

// Standard library headers
#include <fstream>    // For file operations
#include <map>        // For storing configurations and state data
#include <sstream>    // For string formatting
#include <string>     // For text processing
#include <windows.h>  // For MessageBox

// Main implementation file - contains all namespaced code
#include "bare_handed.cpp"

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
    return BareHanded::onStartup(pluginName);
}

/**
 * @brief DynRPG callback: Called on each frame
 * 
 * @param scene Current game scene
 * 
 * @note Used to handle weapon equipping/unequipping based on scene changes
 */
void onFrame(RPG::Scene scene) {
    BareHanded::onFrame(scene);
}

/**
 * @brief DynRPG callback: Called when a variable is changed
 * 
 * @param id Variable ID that is being changed
 * @param value New value for the variable
 * @return bool True to allow the change, false to prevent it
 * 
 * @note Used to detect when variables mapped to actor weapons change
 */
bool onSetVariable(int id, int value) {
    return BareHanded::onSetVariable(id, value);
}

/**
 * @brief DynRPG callback: Called when a comment command is encountered in an event
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
    return BareHanded::onComment(text, parsedData, nextScriptLine, scriptData, eventId, pageId, lineId, nextLineId);
}
