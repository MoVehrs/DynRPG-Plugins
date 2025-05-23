/*
 * Calculation handling for the Limit Break plugin.
 * Contains functions for Limit Break calculations, damage tracking and limit gain.
 */

namespace LimitBreakCalculate
{
// ========================================================================
// Runtime state variables
// ========================================================================

// Tracks previous HP values of all battlers to detect damage/healing between frames
// Key: Pointer to battler, Value: Previous HP value
static std::map<RPG::Battler*, int> preHP;

// Indicates if the next action to check will be from an actor (true) or monster (false)
// Used to determine which damage calculation to apply
static bool nextIsActorAction = false;

// Stores a pointer to the actor who performed the last action
// Used to track which actor should receive limit gain from damage/healing
static RPG::Actor* lastActionActor = nullptr;

// Stores a pointer to the monster who performed the last action
// Used to track which monster caused damage for limit gain calculation
static RPG::Monster* lastActionMonster = nullptr;

// Flag to indicate the system is currently tracking damage for limit calculations
// Set to true during battle actions and reset when processing is complete
static bool monitoringDamage = false;  // Flag to monitor damage between turns

// Flag to track whether we were in battle during the previous frame
// Used to detect battle start and end events
static bool wasInBattle = false;       // Flag to track if we were in battle scene

/**
 * @brief Calculates the equipment-based multiplier for an actor's limit gain
 * 
 * @param actor Pointer to the actor whose equipment multiplier is being calculated
 * @return float The final multiplier value (minimum 0.0, base 1.0)
 * 
 * @note The base multiplier is 1.0. Equipment multipliers are added to this base.
 *       This function checks all equipment slots (weapon, shield, armor, helmet, accessory)
 *       and adds any configured multipliers from the LimitBreakConfig::equipmentMultipliers map.
 *       The result is capped at a minimum of 0.0 (no negative multipliers).
 */
float getEquipmentMultiplier(RPG::Actor* actor) {
    if (!actor) return 1.0f; // Default multiplier if no actor
    
    // Start with base multiplier of 1.0
    float multiplier = 1.0f;
    
    // Check each equipment slot
    short equipIds[] = {
        actor->weaponId,
        actor->shieldId,
        actor->armorId,
        actor->helmetId,
        actor->accessoryId
    };
    
    // Add multipliers for each equipped item found in the configuration
    for (int i = 0; i < 5; ++i) {
        short equipId = equipIds[i];
        if (equipId > 0) { // Valid equipment ID
            auto it = LimitBreakConfig::equipmentMultipliers.find(equipId);
            if (it != LimitBreakConfig::equipmentMultipliers.end()) {
                multiplier += it->second;
            }
        }
    }
    
    // Apply floor cap (no multiplier below 0)
    return std::max(0.0f, multiplier);
}

/**
 * @brief Applies limit gain to an actor based on the current circumstances
 * 
 * @param battler Pointer to the battler (must be an actor) to receive limit gain
 * @param percentGain Amount of limit to add in percent (will be adjusted by equipment multiplier)
 * 
 * @note This function:
 *       1. Verifies the battler is a valid actor with configuration
 *       2. Applies equipment multipliers to the base gain
 *       3. Caps the total limit to 100%
 *       4. Updates the appropriate RPG Maker variable with the new limit value
 *       5. Optionally displays debug information
 */
void applyLimitGain(RPG::Battler* battler, int percentGain) {
    if (!battler || battler->isMonster()) return;

    RPG::Actor* actor = reinterpret_cast<RPG::Actor*>(battler);
    int actorId = actor->id;

    auto cfgIt = LimitBreakConfig::actorConfig.find(actorId);
    if (cfgIt == LimitBreakConfig::actorConfig.end())
        return;

    // Check if actor should be skipped based on mode
    int mode = LimitBreakConfig::getActorMode(actorId);
    if (mode < 0)
        return; // Skip limit gain for this actor

    // Apply equipment multiplier to the percentGain
    float multiplier = getEquipmentMultiplier(actor);
    int adjustedGain = static_cast<int>(percentGain * multiplier);

    int varId = std::get<0>(cfgIt->second);
    int& current = RPG::variables[varId];
    int oldValue = current;
    current = std::min(100, current + adjustedGain);
    
    // Create debug message
    if (LimitBreakConfig::enableDebugMessages) {
        std::string modeStr;
        switch (mode) {
            case 0: modeStr = "Stoic"; break;
            case 1: modeStr = "Warrior"; break;
            case 2: modeStr = "Comrade"; break;
            case 3: modeStr = "Healer"; break;
            case 4: modeStr = "Knight"; break;
            default: modeStr = "Unknown"; break;
        }
        
        std::string debugMessage = "Limit Gain Applied:\n";
        debugMessage += "Actor: " + std::to_string(actorId) + "\n";
        debugMessage += "Mode: " + modeStr + "\n";
        debugMessage += "Base Gain: " + std::to_string(percentGain) + "%\n";
        debugMessage += "Equipment Multiplier: " + std::to_string(multiplier) + "\n";
        debugMessage += "Adjusted Gain: " + std::to_string(adjustedGain) + "%\n";
        debugMessage += "Previous Limit: " + std::to_string(oldValue) + "%\n";
        debugMessage += "New Limit: " + std::to_string(current) + "%\n";
        
        Dialog::Show(debugMessage, "Limit Break - Gain Applied");
    }
}

/**
 * @brief Updates the ultimate limit bar value based on party members' individual limit values
 * 
 * @note This function:
 *       1. Checks if the ultimate limit system is enabled
 *       2. Verifies if the party is at max capacity (either 3 or 4 actors based on config)
 *       3. Calculates the ultimate limit value based on all configured actors
 *       4. Applies special rules to ensure 100% only occurs when all actors are at 100%
 *       5. Updates the RPG Maker variable with the new ultimate limit value
 * 
 *       The ultimate bar percentage is calculated by averaging all actors' limit values,
 *       with unconfigured actors counting as 0% toward the total.
 */
void updateUltimateLimitBar() {
    // If UltimateLimitVarId is 0, the ultimate limit system is disabled
    if (LimitBreakConfig::ultimateLimitVarId <= 0) {
        return;
    }
    
    int requiredPartySize = LimitBreakConfig::useFourActorsForUltimate ? 4 : 3;
    int currentPartySize = 0;
    
    // Count the number of actors in the party
    for (int i = 0; i < 4; ++i) { // Max party size is 4
        if (RPG::Actor::partyMember(i)) {
            currentPartySize++;
        }
    }
    
    // Only calculate if the party is at max capacity
    if (currentPartySize < requiredPartySize) {
        // Party not at required size, set ultimate limit to 0
        RPG::variables[LimitBreakConfig::ultimateLimitVarId] = 0;
        return;
    }
    
    // Party is at required size, calculate the ultimate limit
    int totalLimitValue = 0;
    
    // Sum the limit values of all actors in the battle party
    for (int battlePosIdx = 0; battlePosIdx < requiredPartySize; ++battlePosIdx) {
        RPG::Actor* actor = RPG::Actor::partyMember(battlePosIdx);
        if (!actor) continue; // Should not happen since we checked party size
        
        int actorId = actor->id; // Use the actor's database ID, not the battle position
        auto cfgIt = LimitBreakConfig::actorConfig.find(actorId);
        
        // If actor is not configured, treat their limit value as 0
        if (cfgIt == LimitBreakConfig::actorConfig.end()) {
            // Actor not configured, contributes 0 to the total
            continue;
        }
        
        int limitVarId = std::get<0>(cfgIt->second);
        int limitValue = RPG::variables[limitVarId];
        
        // Add this actor's limit value to the total
        totalLimitValue += limitValue;
    }
    
    // Calculate the percentage fill for the ultimate bar
    // Always divide by the required party size, regardless of how many actors are configured
    int ultimateValue = totalLimitValue / requiredPartySize;
    
    // Only if all actors have 100% limit, ensure ultimate is also 100%
    // This requires checking if all actors are at 100% AND configured
    bool allConfiguredActorsAtMax = true;
    int configuredActorCount = 0;
    
    for (int battlePosIdx = 0; battlePosIdx < requiredPartySize; ++battlePosIdx) {
        RPG::Actor* actor = RPG::Actor::partyMember(battlePosIdx);
        if (!actor) continue;
        
        int actorId = actor->id; // Use the actor's database ID, not the battle position
        auto cfgIt = LimitBreakConfig::actorConfig.find(actorId);
        
        if (cfgIt != LimitBreakConfig::actorConfig.end()) {
            configuredActorCount++;
            int limitVarId = std::get<0>(cfgIt->second);
            int limitValue = RPG::variables[limitVarId];
            
            if (limitValue < 100) {
                allConfiguredActorsAtMax = false;
                break;
            }
        }
    }
    
    // Only set to 100% if all configured actors are at 100% AND at least one actor is configured
    if (allConfiguredActorsAtMax && configuredActorCount > 0 && configuredActorCount == requiredPartySize) {
        ultimateValue = 100;
    }
    
    // Clamp to valid range
    ultimateValue = std::min(100, std::max(0, ultimateValue));
    
    // Update the ultimate limit variable
    RPG::variables[LimitBreakConfig::ultimateLimitVarId] = ultimateValue;
}

/**
 * @brief Analyzes and applies limit gain for actors who heal other party members
 * 
 * @note This function:
 *       1. Only processes if the current actor has Healer mode (mode 3)
 *       2. Calculates total healing done across all targets
 *       3. Applies the Healer formula: (healingDone * 16) / targetMaxHP * multiplier
 *       4. Applies equipment multipliers to the final value
 *       5. Generates debug output if debug messages are enabled
 *       
 *       Healer mode rewards actors for restoring HP to party members,
 *       with stronger healing relative to target max HP giving more limit.
 */
void checkActorHealing() {
    if (!lastActionActor) return;

    int actorId = lastActionActor->id; // Use database ID, not battle position
    int mode = LimitBreakConfig::getActorMode(actorId);

    // Only for Healer mode
    if (mode != 3) return;

    int totalHealing = 0;
    int totalMaxHP = 0;
    bool healingFound = false;
    std::string debugMessage = "Healer Mode Calculation:\n";

    // Calculate total healing done to actors and their max HP
    for (auto& kv : preHP) {
        RPG::Battler* target = kv.first;
        if (!target || target->isMonster()) continue;  // Only actor healing counts

        int before = kv.second;
        int after = target->hp;
        int healing = std::max(0, after - before);  // Positive for healing

        if (healing > 0) {
            healingFound = true;
            totalHealing += healing;

            // Store target max HP
            RPG::Actor* actorTarget = reinterpret_cast<RPG::Actor*>(target);
            totalMaxHP += actorTarget->getMaxHp();
            
            debugMessage += "Actor " + std::to_string(actorTarget->id) + " healed: " + 
                           std::to_string(healing) + " HP (MaxHP: " + 
                           std::to_string(actorTarget->getMaxHp()) + ")\n";
        }
    }

    if (healingFound && totalMaxHP > 0) {
        // Get the equipment multiplier for the actor
        float multiplier = getEquipmentMultiplier(lastActionActor);
        
        // Apply the formula: (healingDone * 16) / targetMaxHP * multiplier
        int gain = static_cast<int>((totalHealing * 16.0f) / totalMaxHP * multiplier);

        if (gain > 0) {
            debugMessage += "\nTotal Healing: " + std::to_string(totalHealing) + 
                           "\nTotal Target MaxHP: " + std::to_string(totalMaxHP) + 
                           "\nEquipment Multiplier: " + std::to_string(multiplier) + 
                           "\nFormula: (totalHealing * 16) / totalMaxHP * multiplier" +
                           "\nLimit Gain: " + std::to_string(gain);
            
            if (LimitBreakConfig::enableDebugMessages) {
                Dialog::Show(debugMessage, "Limit Break - Healing Calculation");
            }
            
            applyLimitGain(lastActionActor, gain);
        }
    }
}

/**
 * @brief Analyzes and applies limit gain for actors who damage monsters
 * 
 * @note This function:
 *       1. Only processes if the current actor has Warrior (mode 1) or Knight (mode 4) mode
 *       2. Calculates damage dealt to each monster as a percentage of their max HP
 *       3. Applies the formula: min(16, (damageDealt * 30) / targetMaxHP) * multiplier
 *       4. For Warrior mode, applies gain from all monster damage
 *       5. For Knight mode, handles only the offensive portion of limit gain
 *       6. Applies equipment multipliers to the final value
 *       
 *       Warrior and Knight modes reward actors for dealing significant damage 
 *       relative to the monster's maximum HP.
 */
void checkActorDamageToMonsters() {
    if (!lastActionActor) return;

    int actorId = lastActionActor->id; // Use database ID, not battle position
    int mode = LimitBreakConfig::getActorMode(actorId);

    // Only for Warrior or Knight modes
    if (mode != 1 && mode != 4) return;

    int gain = 0;
    int maxH = lastActionActor->getMaxHp();

    if (maxH <= 0) return;

    int totalDamageDealt = 0;
    bool damageFound = false;
    std::string debugMessage = "Warrior/Knight Mode Calculation:\n";
    std::string modeStr = (mode == 1) ? "Warrior" : "Knight";
    debugMessage += "Mode: " + modeStr + "\n\n";

    // For tracking total monster maxHP for formula
    int totalTargetMaxHP = 0;
    std::map<RPG::Battler*, std::pair<int, int>> targetDamageAndMaxHP;

    // Calculate total damage dealt to monsters and track target max HP
    for (auto& kv : preHP) {
        RPG::Battler* target = kv.first;
        if (!target || !target->isMonster()) continue;

        int before = kv.second;
        int after = target->hp;
        int delta = std::max(0, before - after);

        if (delta > 0) {
            damageFound = true;
            totalDamageDealt += delta;

            // Store damage and maxHP for each target
            RPG::Monster* monster = reinterpret_cast<RPG::Monster*>(target);
            targetDamageAndMaxHP[target] = std::make_pair(delta, monster->getMaxHp());
            totalTargetMaxHP += monster->getMaxHp();
            
            debugMessage += "Monster " + std::to_string(monster->id) + " damage: " + 
                           std::to_string(delta) + " (MaxHP: " + 
                           std::to_string(monster->getMaxHp()) + ")\n";
        }
    }

    if (damageFound) {
        // Get the equipment multiplier for the actor
        float multiplier = getEquipmentMultiplier(lastActionActor);
        
        // Calculate gain based on damage as percentage of target's max HP
        int totalGainPercent = 0;

        debugMessage += "\nGain calculations per monster:\n";
        
        for (const auto& entry : targetDamageAndMaxHP) {
            int damage = entry.second.first;
            int targetMaxHP = entry.second.second;

            if (targetMaxHP > 0) {
                // Formula: min(16, (damageDealt * 30) / targetMaxHP) * multiplier for each target
                int gainFromTarget = std::min(16, static_cast<int>((damage * 30.0f) / targetMaxHP * multiplier));
                totalGainPercent += gainFromTarget;
                
                RPG::Monster* monster = reinterpret_cast<RPG::Monster*>(entry.first);
                debugMessage += "Monster " + std::to_string(monster->id) + " gain: " + 
                               std::to_string(gainFromTarget) + " (min(16, (" + 
                               std::to_string(damage) + " * 30) / " + 
                               std::to_string(targetMaxHP) + ") * " + 
                               std::to_string(multiplier) + ")\n";
            }
        }

        // Use appropriate final gain based on mode
        if (mode == 1) {  // Warrior
            gain = totalGainPercent;
        } else if (mode == 4) {  // Knight (damage dealt part)
            gain = totalGainPercent;
        }

        if (gain > 0) {
            debugMessage += "\nEquipment Multiplier: " + std::to_string(multiplier) + 
                           "\nTotal Limit Gain: " + std::to_string(gain);
            
            if (LimitBreakConfig::enableDebugMessages) {
                Dialog::Show(debugMessage, "Limit Break - Damage Calculation");
            }
            
            applyLimitGain(lastActionActor, gain);
        }
    }
}

/**
 * @brief Analyzes and applies limit gain for actors who take damage from monsters
 * 
 * @note This function:
 *       1. Calculates damage taken by all actors in the battle party
 *       2. Processes each actor based on their mode:
 *          - Stoic (mode 0): Gains limit from direct damage ((damage * 30) / maxHP * multiplier)
 *          - Comrade (mode 2): Gains limit from damage to OTHER actors ((otherDamage * 20) / maxHP * multiplier)
 *          - Knight (mode 4): Gains limit from direct damage, in addition to offensive limit gain
 *       3. Applies equipment multipliers to all calculations
 *       4. Updates the ultimate limit bar after all limit gains are applied
 *       
 *       This function handles the defensive aspect of limit gain, rewarding
 *       different actors based on their selected battle mode.
 */
void checkMonsterDamageToActors() {
    // For modes that gain limit on damage taken (Stoic, Comrade, Knight)
    bool anyDamageDetected = false;
    std::string debugMessage = "Monster Damage Calculation:\n\n";

    // First, calculate total damage to all actors for Comrade mode
    int totalGroupDamage = 0;
    std::map<RPG::Actor*, int> actorDamage;

    for (auto& kv : preHP) {
        RPG::Battler* a = kv.first;
        if (!a || a->isMonster()) continue;

        int before = kv.second;
        int after = a->hp;
        int delta = std::max(0, before - after);

        if (delta > 0) {
            anyDamageDetected = true;
            totalGroupDamage += delta;

            RPG::Actor* actor = reinterpret_cast<RPG::Actor*>(a);
            actorDamage[actor] = delta;
            
            debugMessage += "Actor " + std::to_string(actor->id) + " took " + 
                           std::to_string(delta) + " damage (MaxHP: " + 
                           std::to_string(actor->getMaxHp()) + ")\n";
        }
    }

    debugMessage += "\nTotal group damage: " + std::to_string(totalGroupDamage) + "\n\n";
    debugMessage += "Limit gain calculations:\n";

    // Now process each actor for all modes
    for (int i = 1; i <= RPG::actors.count(); ++i) {
        RPG::Actor* actor = RPG::actors[i];
        if (!actor) continue;

        int actorId = actor->id; // Use database ID, not battle position
        int mode = LimitBreakConfig::getActorMode(actorId);

        // Skip actors with negative mode values
        if (mode < 0) continue;

        std::string modeStr;
        switch (mode) {
            case 0: modeStr = "Stoic"; break;
            case 2: modeStr = "Comrade"; break;
            case 4: modeStr = "Knight"; break;
            default: modeStr = "Unknown"; break;
        }

        debugMessage += "\nActor " + std::to_string(actorId) + " (" + modeStr + " mode):\n";

        int gain = 0;
        int maxH = actor->getMaxHp();
        if (maxH <= 0) continue;

        // Get the equipment multiplier for this actor
        float multiplier = getEquipmentMultiplier(actor);

        // Get this actor's damage (if any)
        int actorDelta = 0;
        auto dmgIt = actorDamage.find(actor);
        if (dmgIt != actorDamage.end()) {
            actorDelta = dmgIt->second;
        }

        switch (mode) {
            case 0: // Stoic
                if (actorDelta > 0) {
                    gain = static_cast<int>((actorDelta * 30.0f) / maxH * multiplier);
                    debugMessage += "  Stoic formula: (" + std::to_string(actorDelta) + 
                                   " * 30) / " + std::to_string(maxH) + " * " + 
                                   std::to_string(multiplier) + " = " + std::to_string(gain) + "\n";
                }
                break;

            case 2: { // Comrade
                // Calculate damage to all OTHER actors
                int otherActorsDamage = totalGroupDamage - actorDelta;

                // Apply gain even if this actor wasn't hit
                if (otherActorsDamage > 0) {
                    gain = static_cast<int>((otherActorsDamage * 20.0f) / maxH * multiplier);
                    debugMessage += "  Comrade formula: (" + std::to_string(otherActorsDamage) + 
                                   " * 20) / " + std::to_string(maxH) + " * " + 
                                   std::to_string(multiplier) + " = " + std::to_string(gain) + "\n";
                }
                break;
            }

            case 4: // Knight - handle damage taken part here
                if (actorDelta > 0) {
                    gain = static_cast<int>((actorDelta * 30.0f) / maxH * multiplier);
                    debugMessage += "  Knight formula: (" + std::to_string(actorDelta) + 
                                   " * 30) / " + std::to_string(maxH) + " * " + 
                                   std::to_string(multiplier) + " = " + std::to_string(gain) + "\n";
                }
                break;

            default:
                break;
        }

        if (gain > 0) {
            debugMessage += "  Final gain: " + std::to_string(gain) + "\n";
            applyLimitGain(actor, gain);
        } else {
            debugMessage += "  No gain\n";
        }
    }

    if (anyDamageDetected && LimitBreakConfig::enableDebugMessages) {
        Dialog::Show(debugMessage, "Limit Break - Monster Damage Calculation");
        
        // Update the ultimate limit bar
        updateUltimateLimitBar();
    }
}

/**
 * @brief Main damage detection and limit gain application function
 * 
 * @note This function:
 *       1. Monitors HP changes between frames during battle actions
 *       2. Detects different types of actions:
 *          - Actors damaging monsters
 *          - Actors healing other actors
 *          - Monsters damaging actors
 *       3. Delegates to specialized handlers (checkActorDamageToMonsters, checkActorHealing, 
 *          checkMonsterDamageToActors) to apply the appropriate limit gain
 *       4. Updates the ultimate limit bar after limit gain is applied
 *       5. Maintains HP tracking between frames for multi-hit actions
 *
 *       This is the central damage detection system that runs every frame while
 *       monitoringDamage is active.
 */
void checkDamageAndApplyGain() {
    if (!monitoringDamage) return;

    bool damageFound = false;
    std::string debugMessage = "";

    if (nextIsActorAction) {
        // Actor action - check for damage to monsters and healing to actors
        // Store the current state of HP for comparison on next frame
        std::map<RPG::Battler*, int> currentHP;
        
        // Record current HP of monsters
        for (int i = 0; i < RPG::monsters.count(); ++i) {
            RPG::Monster* m = RPG::monsters[i];
            if (m) currentHP[m] = m->hp;
        }
        
        // Record current HP of actors (for healing detection)
        for (int i = 1; i <= RPG::actors.count(); ++i) {
            RPG::Actor* a = RPG::actors[i];
            if (a) currentHP[a] = a->hp;
        }
        
        // Check for damage to monsters
        for (auto& kv : preHP) {
            RPG::Battler* target = kv.first;
            if (!target || !target->isMonster()) continue;
            
            int before = kv.second;
            int after = target->hp;
            int delta = std::max(0, before - after);
            
            if (delta > 0) {
                damageFound = true;
                RPG::Monster* monster = reinterpret_cast<RPG::Monster*>(target);
                debugMessage += "Monster " + std::to_string(monster->id) + " took " + std::to_string(delta) + " damage\n";
                break;  // Found damage, no need to check further
            }
        }
        
        // Check for healing to actors
        if (!damageFound) {
            for (auto& kv : preHP) {
                RPG::Battler* target = kv.first;
                if (!target || target->isMonster()) continue;
                
                int before = kv.second;
                int after = target->hp;
                int healing = std::max(0, after - before);
                
                if (healing > 0) {
                    damageFound = true;
                    RPG::Actor* actor = reinterpret_cast<RPG::Actor*>(target);
                    debugMessage += "Actor " + std::to_string(actor->id) + " healed " + std::to_string(healing) + " HP\n";
                    break;  // Found healing, no need to check further
                }
            }
        }
        
        // If damage/healing was found, apply the appropriate gain
        if (damageFound) {
            // Show debug message box if enabled
            if (LimitBreakConfig::enableDebugMessages && lastActionActor) {
                std::string actionType = "Actor " + std::to_string(lastActionActor->id) + " action detected:\n";
                Dialog::Show(actionType + debugMessage, "Limit Break Debug");
            }
            
            checkActorDamageToMonsters();
            checkActorHealing();
            
            // Update preHP with current HP for next frame's comparison
            preHP = currentHP;
            
            // Update the ultimate limit bar
            updateUltimateLimitBar();
        }
    } else {
        // Monster action - check for damage to actors
        std::map<RPG::Battler*, int> currentHP;
        
        // Record current HP of actors
        for (int i = 1; i <= RPG::actors.count(); ++i) {
            RPG::Actor* a = RPG::actors[i];
            if (a) currentHP[a] = a->hp;
        }
        
        // Check for damage to actors
        for (auto& kv : preHP) {
            RPG::Battler* target = kv.first;
            if (!target || target->isMonster()) continue;
            
            int before = kv.second;
            int after = target->hp;
            int delta = std::max(0, before - after);
            
            if (delta > 0) {
                damageFound = true;
                RPG::Actor* actor = reinterpret_cast<RPG::Actor*>(target);
                debugMessage += "Actor " + std::to_string(actor->id) + " took " + std::to_string(delta) + " damage\n";
                break;  // Found damage, no need to check further
            }
        }
        
        // If damage was found, apply the appropriate gain
        if (damageFound) {
            // Show debug message box if enabled
            if (LimitBreakConfig::enableDebugMessages && lastActionMonster) {
                std::string actionType = "Monster " + std::to_string(lastActionMonster->id) + " action detected:\n";
                Dialog::Show(actionType + debugMessage, "Limit Break Debug");
            }
            
            checkMonsterDamageToActors();
            
            // Update preHP with current HP for next frame's comparison
            preHP = currentHP;
            
            // Update the ultimate limit bar
            updateUltimateLimitBar();
        }
    }
}

} // namespace LimitBreakCalculate 