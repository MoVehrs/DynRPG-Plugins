/*
 * Entry point for the DirectSkills plugin for RPG Maker 2003.
 * This file contains DynRPG callback functions that serve as entry
 * points from the game engine into our plugin's functionality.
 * 
 * All implementation details are organized in the DirectSkills namespace.
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
#include "direct_skills.cpp"

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
    return DirectSkills::onStartup(pluginName);
}

/**
 * @brief DynRPG callback: Before battler action
 * 
 * @param battler Pointer to the battler (actor or monster) performing the action
 * @param firstTry Whether this is the first attempt at performing the action
 * @return bool Always returns true to continue normal processing
 * 
 * @note Used to swap regular attacks to skills when using configured battle commands
 */
bool onDoBattlerAction(RPG::Battler* battler, bool firstTry) {
    return DirectSkills::onDoBattlerAction(battler, firstTry);
}
