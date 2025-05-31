/**
 * @file main.cpp
 * @brief Entry point for the DirectSkills plugin for RPG Maker 2003.
 * @details This file contains DynRPG callback functions that serve as entry
 *          points from the game engine into the plugin's functionality.
 *          All implementation details are organized in the DirectSkills namespace.
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
#include "direct_skills.cpp"

/**
 * @defgroup callbacks DynRPG Plugin Callbacks
 * @brief Global callback functions called by the DynRPG system
 * @{
 */

/**
 * @brief Initializes the DirectSkills plugin.
 * @param pluginName Name of the plugin section in DynRPG.ini.
 * @return True if initialization succeeded, false if it failed.
 * @note Called once when the plugin is first loaded.
 * @see DirectSkills::onStartup
 */
bool onStartup(char *pluginName) {
    return DirectSkills::onStartup(pluginName);
}

/**
 * @brief Performs cleanup when the plugin is unloaded.
 * @note Called when the plugin is being unloaded or the game exits.
 * @see DirectSkills::onExit
 */
void onExit() {
    DirectSkills::onExit();
}

/**
 * @brief Processes frame updates during gameplay.
 * @param scene The current game scene.
 * @note Tracks the selected command in the battle command window
 *       and updates the persistent command selection state.
 * @see DirectSkills::onFrame
 */
void onFrame(RPG::Scene scene) {
    DirectSkills::onFrame(scene);
}

/**
 * @brief Processes battler actions before they are executed.
 * @param battler Pointer to the battler (actor or monster) performing the action.
 * @param firstTry Whether this is the first attempt at performing the action.
 * @return True to continue normal processing.
 * @note Checks if the action should be replaced with a skill based on the selected command.
 * @see DirectSkills::onDoBattlerAction
 */
bool onDoBattlerAction(RPG::Battler* battler, bool firstTry) {
    return DirectSkills::onDoBattlerAction(battler, firstTry);
}

/**
 * @brief Processes cleanup after a battler's action completes.
 * @param battler Pointer to the battler that just completed an action.
 * @param success Whether the action was successful.
 * @return True to continue normal processing.
 * @note Resets the persistent selected command to prevent command selection
 *       from bleeding over between different actors' turns.
 * @see DirectSkills::onBattlerActionDone
 */
bool onBattlerActionDone(RPG::Battler* battler, bool success) {
    return DirectSkills::onBattlerActionDone(battler, success);
}

/** @} */ // end of callbacks group
