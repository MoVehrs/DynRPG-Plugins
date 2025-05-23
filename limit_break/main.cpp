/*
 * Entry point for the Limit Break plugin for RPG Maker 2000/2003.
 * This file contains DynRPG callback functions that serve as entry
 * points from the game engine into our plugin's functionality.
 * 
 * All implementation details are organized in the LimitBreak namespace.
 */

// Core DynRPG header
#include <DynRPG/DynRPG.h>

// Standard library headers
#include <algorithm>  // For std::min, std::max
#include <map>        // For storing configurations and state data
#include <tuple>      // For storing actor configurations
#include <string>     // For text processing
#include <sstream>    // For string formatting
#include <vector>     // For storing animation frames
#include <windows.h>  // For MessageBox
#include <fstream>    // For file operations

// Main implementation file - contains all namespaced code
#include "limit_break.cpp"

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
    return LimitBreak::onStartup(pluginName);
}

/**
 * @brief DynRPG callback: Battle status window drawing
 * 
 * @param x The index of the actor (0-3) in the battle party
 * @param selection Current command selection index
 * @param selActive Whether the selection is active
 * @param isTargetSelection Whether target selection is active
 * @param isVisible Whether the window is visible
 * @return bool Always returns true to continue normal processing
 * 
 * @note Used to detect when the Limit command is selected
 */
bool onDrawBattleStatusWindow(int x, int selection, bool selActive, bool isTargetSelection, bool isVisible) {
    return LimitBreak::onDrawBattleStatusWindow(x, selection, selActive, isTargetSelection, isVisible);
}

/**
 * @brief DynRPG callback: Battle action window drawing
 * 
 * @param x Pointer to X coordinate of the window
 * @param y Pointer to Y coordinate of the window
 * @param selection Current command selection index
 * @param selActive Whether the selection is active
 * @param isVisible Whether the window is visible
 * @return bool Always returns true to continue normal processing
 * 
 * @note Used to draw the ultimate limit bar
 */
bool onDrawBattleActionWindow(int* x, int* y, int selection, bool selActive, bool isVisible) {
    return LimitBreak::onDrawBattleActionWindow(x, y, selection, selActive, isVisible);
}

/**
 * @brief DynRPG callback: Before battler action
 * 
 * @param battler Pointer to the battler (actor or monster) performing the action
 * @param firstTry Whether this is the first attempt at performing the action
 * @return bool Always returns true to continue normal processing
 * 
 * @note Used to swap regular attacks to limit breaks when conditions are met
 *       Also sets up damage monitoring for limit gain calculation
 */
bool onDoBattlerAction(RPG::Battler* battler, bool firstTry) {
    return LimitBreak::onDoBattlerAction(battler, firstTry);
}

/**
 * @brief DynRPG callback: After battler action
 * 
 * @param battler Pointer to the battler (actor or monster) that performed the action
 * @param success Whether the action was successful
 * @return bool Always returns true to continue normal processing
 * 
 * @note Used to start damage monitoring for multi-hit attacks and skills
 */
bool onBattlerActionDone(RPG::Battler* battler, bool success) {
    return LimitBreak::onBattlerActionDone(battler, success);
}

/**
 * @brief DynRPG callback: Every frame update
 * 
 * @param scene Current game scene
 * 
 * @note Used to track damage between frames and update limit gauges
 *       Also monitors battle start/end to properly initialize and cleanup
 */
void onFrame(RPG::Scene scene) {
    LimitBreak::onFrame(scene);
}

/**
 * @brief DynRPG callback: Game exit
 * 
 * @note Used to free resources when the game is closing
 */
void onExit() {
    LimitBreak::onExit();
}
