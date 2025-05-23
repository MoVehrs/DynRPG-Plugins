/*
 * Main implementation file for the Limit Break plugin.
 * Contains the primary LimitBreak namespace with all callback implementations
 * and connects the different modular components into a unified system.
 */

// Core DynRPG header
#include <DynRPG/DynRPG.h>

// Include our modular code
#include "dialog.cpp"
#include "limit_break_config.cpp"
#include "limit_break_calculate.cpp"
#include "limit_break_graphics.cpp"

// Main limit break namespace to handle callbacks
namespace LimitBreak
{
// Flag to track if Limit command was selected
static bool limitCommandSelected = false;

// Flag to track if Ultimate Limit command was selected
static bool ultimateLimitCommandSelected = false;

/**
 * @brief Initializes the Limit Break plugin
 * 
 * @param pluginName Name of the plugin section in DynRPG.ini
 * @return bool True if configuration loaded successfully, false otherwise
 * 
 * @note Called once at plugin startup
 *       Delegates to LimitBreakConfig::LoadConfig to read all settings
 */
bool onStartup(char *pluginName) {
    return LimitBreakConfig::LoadConfig(pluginName);
}

/**
 * @brief Monitors the battle command selection
 * 
 * @param x The battle position index (0-3) of the actor
 * @param selection The selected command index (0 is first command)
 * @param selActive Whether the selection is currently active
 * @param isTargetSelection Whether the player is selecting a target
 * @param isVisible Whether the window is visible
 * @return bool Always returns true to continue normal processing
 * 
 * @note Detects when the Limit command is selected by checking if:
 *       1. The window is active (not target selection mode)
 *       2. The actor's commands include the configured Limit command
 *       3. The corresponding command index is selected
 */
bool onDrawBattleStatusWindow(int x, int selection, bool selActive, bool isTargetSelection, bool isVisible) {
    // Only process when the command window is active (not target selection)
    if (!isTargetSelection && selActive && isVisible) {
        // Check if the actor has battle commands
        RPG::Actor* actor = RPG::Actor::partyMember(x);
        if (!actor) return true;

        // Note: x is the battle position index (0-3), but we need to use actor->id (database ID)
        // when looking up configurations in actorConfig
        
        // Initialize command selection flags to false
        limitCommandSelected = false;
        ultimateLimitCommandSelected = false;
        
        // Check if the first command is the "Limit" command
        // In DynRPG, battleCommands is an array of command IDs, not objects
        if (actor->battleCommands[0] == LimitBreakConfig::limitCommandId) {
            // Track if the first command (Limit) is selected
            limitCommandSelected = (selection == 0);
        }
        
        // Check if the first command is the "Ultimate Limit" command
        if (LimitBreakConfig::ultimateLimitCommandId > 0 && 
            actor->battleCommands[0] == LimitBreakConfig::ultimateLimitCommandId) {
            // Track if the first command (Ultimate Limit) is selected
            ultimateLimitCommandSelected = (selection == 0);
        }
    }
    return true;
}

/**
 * @brief Handles drawing of the battle action window
 * 
 * @param x Pointer to X coordinate of the window
 * @param y Pointer to Y coordinate of the window
 * @param selection Current command selection index
 * @param selActive Whether the selection is active
 * @param isVisible Whether the window is visible
 * @return bool Always returns true to continue normal processing
 * 
 * @note Used to draw the ultimate limit bar on top of all battle elements
 *       The drawing itself is delegated to LimitBreakGraphics::drawUltimateLimitBar
 */
bool onDrawBattleActionWindow(int* x, int* y, int selection, bool selActive, bool isVisible) {
    // Draw the ultimate limit bar on top of all battle elements
    LimitBreakGraphics::drawUltimateLimitBar();
    return true;
}

/**
 * @brief Processes battler actions before they occur
 * 
 * @param battler Pointer to the battler (actor or monster) performing the action
 * @param firstTry Whether this is the first attempt at processing this action
 * @return bool Always returns true to continue normal processing
 * 
 * @note This function:
 *       1. Stops any ongoing damage monitoring from previous actions
 *       2. Records which battler is performing the action
 *       3. For actors using the Limit command, checks if limit gauge is at 100%
 *          If so, converts the action from Attack to a specific Limit Skill
 *       4. Records the HP of all relevant battlers before the action
 *          to later detect changes for limit gain calculation
 */
bool onDoBattlerAction(RPG::Battler* battler, bool firstTry) {
    // Original damage monitoring code
    if (!firstTry) return true;

    std::string actionType;
    std::string battlerInfo;
    
    if (battler->isMonster()) {
        RPG::Monster* monster = reinterpret_cast<RPG::Monster*>(battler);
        battlerInfo = "Monster " + std::to_string(monster->id);
    } else {
        RPG::Actor* actor = reinterpret_cast<RPG::Actor*>(battler);
        battlerInfo = "Actor " + std::to_string(actor->id);
    }

    // If we're monitoring damage, stop monitoring
    if (LimitBreakCalculate::monitoringDamage) {
        LimitBreakCalculate::monitoringDamage = false;
        
        if (LimitBreakConfig::enableDebugMessages) {
            std::string debugMessage = "Damage Monitoring Stopped\n";
            debugMessage += "New Action Starting: " + battlerInfo;
            Dialog::Show(debugMessage, "Limit Break - Monitoring Status");
        }
    }

    // Now proceed with the new action
    LimitBreakCalculate::nextIsActorAction = !battler->isMonster();

    // Store the battler who is performing the action
    if (LimitBreakCalculate::nextIsActorAction) {
        LimitBreakCalculate::lastActionActor = reinterpret_cast<RPG::Actor*>(battler);
        LimitBreakCalculate::lastActionMonster = nullptr;

        // Direct check for the Limit command as the first command
        bool isLimitCommand = false;
        if (LimitBreakCalculate::lastActionActor && LimitBreakCalculate::lastActionActor->battleCommands[0] == LimitBreakConfig::limitCommandId) {
            isLimitCommand = true;
        }

        // Check if this is a Limit action that needs to be swapped
        // Use either the flag or direct command check
        if ((limitCommandSelected || isLimitCommand) && LimitBreakCalculate::lastActionActor) {
            // Get the current action
            RPG::Action* action = LimitBreakCalculate::lastActionActor->action;
            
            // Only convert Attack/Double Attack commands to limit skills
            if (action && action->kind == RPG::AK_BASIC && 
                (action->basicActionId == RPG::BA_ATTACK || action->basicActionId == RPG::BA_DOUBLE_ATTACK)) {
                
                // Get the actor's limit variable and skill ID
                int actorId = LimitBreakCalculate::lastActionActor->id; // Use database ID, not battle position
                auto cfgIt = LimitBreakConfig::actorConfig.find(actorId);
                if (cfgIt != LimitBreakConfig::actorConfig.end()) {
                    int limitVarId = std::get<0>(cfgIt->second);
                    int limitValue = RPG::variables[limitVarId];
                    
                    // Get the actor's limit skill ID using the helper function
                    int limitSkillId = LimitBreakConfig::getActorLimitSkillId(actorId);
                    
                    // Only allow limit break if the limit gauge is at 100%
                    if (limitValue >= 100) {
                        // Swap the action to use the actor's specific limit break skill
                        action->kind = RPG::AK_SKILL;
                        action->skillId = limitSkillId;
                        
                        if (LimitBreakConfig::enableDebugMessages) {
                            std::string debugMessage = "Limit Break Used!\n";
                            debugMessage += "Actor: " + std::to_string(actorId) + "\n";
                            debugMessage += "Skill ID: " + std::to_string(limitSkillId) + "\n";
                            debugMessage += "Limit gauge reset from 100% to 0%";
                            Dialog::Show(debugMessage, "Limit Break - Limit Used");
                        }
                        
                        // Reset the limit gauge to 0
                        RPG::variables[limitVarId] = 0;
                        
                        // Update the ultimate limit bar after using a limit break
                        LimitBreakCalculate::updateUltimateLimitBar();

                        // Update battle events to refresh command display
                        RPG::updateBattleEvents(RPG::BEUM_BATTLE_START, nullptr);
                    }
                }
                
                // Reset the flag
                limitCommandSelected = false;
            }
        }
        
        // Direct check for the Ultimate Limit command as the first command
        bool isUltimateLimitCommand = false;
        if (LimitBreakConfig::ultimateLimitCommandId > 0 && 
            LimitBreakCalculate::lastActionActor && 
            LimitBreakCalculate::lastActionActor->battleCommands[0] == LimitBreakConfig::ultimateLimitCommandId) {
            isUltimateLimitCommand = true;
        }
        
        // Check if this is an Ultimate Limit action that needs to be swapped
        // Use either the flag or direct command check
        if ((ultimateLimitCommandSelected || isUltimateLimitCommand) && LimitBreakCalculate::lastActionActor) {
            // Get the current action
            RPG::Action* action = LimitBreakCalculate::lastActionActor->action;
            
            // Only convert Attack/Double Attack commands to ultimate limit skills
            if (action && action->kind == RPG::AK_BASIC && 
                (action->basicActionId == RPG::BA_ATTACK || action->basicActionId == RPG::BA_DOUBLE_ATTACK)) {
                
                // Get the actor's ID and the ultimate limit value
                int actorId = LimitBreakCalculate::lastActionActor->id;
                int ultimateValue = RPG::variables[LimitBreakConfig::ultimateLimitVarId];
                
                // Get the actor's ultimate limit skill ID
                int ultimateLimitSkillId = LimitBreakConfig::getActorUltimateLimitSkillId(actorId);
                
                // Only allow ultimate limit break if the ultimate gauge is at 100% and the actor has a configured skill
                if (ultimateValue >= 100 && ultimateLimitSkillId > 0) {
                    // Swap the action to use the actor's specific ultimate limit break skill
                    action->kind = RPG::AK_SKILL;
                    action->skillId = ultimateLimitSkillId;
                    
                    // Reset the ultimate limit bar to 0
                    RPG::variables[LimitBreakConfig::ultimateLimitVarId] = 0;
                    
                    // Reset all party members' limit gauges to 0
                    // Use the configured party size (3 or 4 actors)
                    int maxPartyMembers = LimitBreakConfig::useFourActorsForUltimate ? 4 : 3;
                    for (int i = 0; i < maxPartyMembers; i++) {
                        RPG::Actor* partyMember = RPG::Actor::partyMember(i);
                        if (partyMember) {
                            int memberId = partyMember->id;
                            auto cfgIt = LimitBreakConfig::actorConfig.find(memberId);
                            if (cfgIt != LimitBreakConfig::actorConfig.end()) {
                                int limitVarId = std::get<0>(cfgIt->second);
                                RPG::variables[limitVarId] = 0;
                            }
                        }
                    }
                    
                    // Update the ultimate limit bar after resetting all values
                    LimitBreakCalculate::updateUltimateLimitBar();

                    // Update battle events to refresh command display
                    RPG::updateBattleEvents(RPG::BEUM_BATTLE_START, nullptr);
                    
                    if (LimitBreakConfig::enableDebugMessages) {
                        std::string debugMessage = "Ultimate Limit Break Used!\n";
                        debugMessage += "Actor: " + std::to_string(actorId) + "\n";
                        debugMessage += "Ultimate Skill ID: " + std::to_string(ultimateLimitSkillId) + "\n";
                        debugMessage += "Ultimate gauge and all actor limit gauges reset to 0%";
                        Dialog::Show(debugMessage, "Limit Break - Ultimate Limit Used");
                    }
                    
                    // Reset the flag
                    ultimateLimitCommandSelected = false;
                }
                
                // Reset the flag
                ultimateLimitCommandSelected = false;
            }
        }
    } else {
        LimitBreakCalculate::lastActionActor = nullptr;
        LimitBreakCalculate::lastActionMonster = reinterpret_cast<RPG::Monster*>(battler);
    }

    // Record HP of all relevant battlers before action
    LimitBreakCalculate::preHP.clear();

    // Always record HP of all actors for both actor and monster actions
    // This is needed for both damage tracking and healing tracking
    for (int i = 1; i <= RPG::actors.count(); ++i) { // Actor IDs start at 1
        RPG::Actor* a = RPG::actors[i];
        if (a) LimitBreakCalculate::preHP[a] = a->hp;
    }

    // Record monster HP only when an actor acts (for damage tracking)
    if (LimitBreakCalculate::nextIsActorAction) {
        for (int i = 0; i < RPG::monsters.count(); ++i) {
            RPG::Monster* m = RPG::monsters[i];
            if (m) LimitBreakCalculate::preHP[m] = m->hp;
        }
    }

    return true;
}

/**
 * @brief Processes battler actions after they complete
 * 
 * @param battler Pointer to the battler (actor or monster) that performed the action
 * @param success Whether the action completed successfully
 * @return bool Always returns true to continue normal processing
 * 
 * @note This function:
 *       1. Starts damage monitoring to track HP changes for limit gain
 *       2. The actual damage detection occurs in the onFrame callback
 *       3. Records which battler performed the action for proper attribution
 *       4. Updates the ultimate limit bar after the action completes
 */
bool onBattlerActionDone(RPG::Battler* battler, bool success) {
    if (!success) return true;

    std::string battlerInfo;
    
    if (battler->isMonster()) {
        RPG::Monster* monster = reinterpret_cast<RPG::Monster*>(battler);
        battlerInfo = "Monster " + std::to_string(monster->id);
    } else {
        RPG::Actor* actor = reinterpret_cast<RPG::Actor*>(battler);
        battlerInfo = "Actor " + std::to_string(actor->id);
    }

    // For both actor and monster actions, start monitoring damage
    if (battler) {
        LimitBreakCalculate::monitoringDamage = true;
        
        if (LimitBreakConfig::enableDebugMessages) {
            std::string debugMessage = "Damage Monitoring Started\n";
            debugMessage += "Action Completed: " + battlerInfo + "\n";
            debugMessage += "Monitoring for multi-hit damage/healing...";
            Dialog::Show(debugMessage, "Limit Break - Monitoring Status");
        }
    }

    // Update the ultimate limit bar after an action
    LimitBreakCalculate::updateUltimateLimitBar();

    return true;
}

/**
 * @brief Frame update callback that runs every game frame
 * 
 * @param scene Current game scene
 * 
 * @note This function:
 *       1. Detects battle scene transitions (entering/exiting battle)
 *       2. Initializes the ultimate limit bar upon battle entry
 *       3. Monitors for damage during active battles
 *       4. Resets the ultimate limit bar to 0 when exiting battle
 *       5. Handles tracking for sound effect playback at 100% limit
 */
void onFrame(RPG::Scene scene) {
    // Track battle scene transitions
    bool isInBattle = (scene == RPG::SCENE_BATTLE);
    
    // Only process in battle scenes
    if (scene != RPG::SCENE_BATTLE) {
        // If we leave battle while monitoring damage, clean up
        if (LimitBreakCalculate::monitoringDamage) {
            LimitBreakCalculate::monitoringDamage = false;
        }
        
        // Outside of battle, reset the ultimate limit bar to 0
        if (LimitBreakCalculate::wasInBattle && !isInBattle) {
            // Just exited battle
            RPG::variables[LimitBreakConfig::ultimateLimitVarId] = 0;
            // Reset sound playback tracking flag
            LimitBreakConfig::wasAt100Percent = false;
        }
    } else {
        // In battle scene
        
        // If we just entered battle, initialize the ultimate limit bar
        if (!LimitBreakCalculate::wasInBattle) {
            // Calculate initial value for the ultimate bar
            LimitBreakCalculate::updateUltimateLimitBar();
            // Reset sound playback tracking flag
            LimitBreakConfig::wasAt100Percent = false;
        }
        
        // If we're monitoring for damage, check each frame for HP changes
        if (LimitBreakCalculate::monitoringDamage) {
            LimitBreakCalculate::checkDamageAndApplyGain();
        }
    }

    // Update the battle state tracker
    LimitBreakCalculate::wasInBattle = isInBattle;
}

/**
 * @brief Cleanup resources when the game exits
 * 
 * @note Ensures proper release of all allocated resources,
 *       particularly image resources used by the ultimate limit bar
 */
void onExit() {
    LimitBreakGraphics::freeUltimateBarImages();
}

} // namespace LimitBreak