# Bare-Handed Plugin for RPG Maker 2003 (DynRPG)

This DynRPG plugin allows actors without equipped weapons to automatically equip a configurable "unarmed" weapon in RPG Maker 2003, giving them proper stats for barehanded combat.

## Features

- Fully configurable through DynRPG.ini.
- Per-actor unarmed weapon IDs.
- Variable-based unarmed weapon IDs.
- Automatic equipping and unequipping of unarmed weapons based on game scene.
- Debug message option for troubleshooting.

## Installation

1. Place the `bare_handed.dll` file in your game's DynPlugins folder.
2. Configure the plugin in your `DynRPG.ini` file (see below).

## Configuration

Configure the plugin in `DynRPG.ini` using the following format:

```ini
[bare_handed]
; (OPTIONAL) Enables or disables debug message boxes for configuration
; false = No debug messages for configuration (default)
; true = Show detailed message boxes when loading settings from DynRPG.ini
EnableDebugConfig=false

; (OPTIONAL) Enables or disables debug message boxes for runtime operations
; false = No debug messages during gameplay (default)
; true = Show detailed message boxes when equipping/unequipping weapons
EnableDebugRuntime=false

; (OPTIONAL) Maximum actor ID to check for configuration
; Default is 20 if not specified
; Increase this value if your game uses actor IDs higher than 20
MaxActorId=20

; Actor-specific unarmed weapon configurations
; Format: ActorX_UnarmedWeaponId=Y
; Where X is the actor ID and Y is the weapon ID to equip when unarmed
; Examples:
; Actor1_UnarmedWeaponId=87  ; Actor 1 will equip weapon ID 87 when unarmed
; Actor2_UnarmedWeaponId=88  ; Actor 2 will equip weapon ID 88 when unarmed

; Variable-based weapon ID configurations
; Format: ActorX_VariableId=Y
; Where X is the actor ID and Y is the variable ID containing the weapon ID to equip
; Examples:
; Actor1_VariableId=10       ; Variable 10 contains the weapon ID to equip for Actor 1

; Configure your actor settings below:
Actor1_UnarmedWeaponId=87
Actor2_UnarmedWeaponId=88
Actor3_UnarmedWeaponId=89

; Variable-based weapon ID example
Actor4_VariableId=10  ; Variable 10 contains the weapon ID to equip for Actor 4
```

### Debug Options

- `EnableDebugConfig`: Whether to display debug message boxes during configuration loading (**OPTIONAL**).
  - false = No debug messages (default).
  - true = Show detailed message boxes when loading settings from DynRPG.ini.
  - Useful for verifying that actor configurations are properly loaded.

- `EnableDebugRuntime`: Whether to display debug message boxes during gameplay (**OPTIONAL**).
  - false = No debug messages (default).
  - true = Show detailed message boxes when equipping/unequipping weapons.
  - Useful for monitoring when weapons are automatically equipped or unequipped.

### Actor Configuration

The plugin requires one or more actor-to-weapon mappings to function. Each mapping follows this format:

```ini
[bare_handed]
ActorX_UnarmedWeaponId=Y
```

Where:
- `X` is the actor ID from your RPG Maker 2003 database.
- `Y` is the weapon ID that should be equipped when this actor has no weapon.

For example:
```ini
[bare_handed]
Actor1_UnarmedWeaponId=87
```

This means that when actor 1 has no weapon equipped, the plugin will automatically equip weapon ID 87 during map scenes.

**Important Notes**:
- Only positive weapon IDs are valid. Zero or negative values will be ignored.
- Actors without a specific configuration will not have an unarmed weapon equipped.
- You must create the appropriate weapons in your database for this plugin to work.

### Variable-Based Weapon IDs

For dynamic weapon configuration, you can use variable-based weapon IDs. The format is:

```ini
[bare_handed]
ActorX_VariableId=Y
```

Where:
- `X` is the actor ID from your RPG Maker 2003 database.
- `Y` is the variable ID that contains the weapon ID to equip.

For example:
```ini
[bare_handed]
Actor1_VariableId=10
```

This means:
- The value in variable 10 will be used as the weapon ID to equip for actor 1 when unarmed.
- When variable 10 contains a positive value, that value is used as the weapon ID.
- When variable 10 contains 0 or a negative value, no weapon will be equipped (the actor is treated as unconfigured).

**Important Note**: The variable ID must be a positive value for this feature to work. If the variable ID is invalid or the variable contains a non-positive value, the actor will be treated as if they have no unarmed weapon configuration.

## How It Works

1. When on the map screen, the plugin automatically equips the configured unarmed weapon for any actor who has no weapon equipped.
   - For dual-wielding actors (with `Two Wpn.` flag set), the plugin checks both weapon slots (weapon and shield) and only equips a bare hand weapon if both slots are empty.
   - The plugin will never equip a bare hand weapon in the secondary (shield) slot. It only checks this slot to determine if a bare hand weapon should be equipped in the primary slot.
2. When entering the menu or shop screen, the plugin automatically unequips these unarmed weapons to allow normal equipment changes.

## Dual-Wielding Support

The plugin provides comprehensive support for dual-wielding characters (those with the `Two Wpn.` flag set in the database):

### How It Works with Dual-Wielders

- **Weapon Slot Behavior**:
  - The plugin will only equip a bare hand weapon in the primary slot (weaponId).
  - The secondary slot (shieldId) is never automatically filled by the plugin.
  - A bare hand weapon will only be equipped if both weapon slots are empty.

- **Unequipping Behavior**:
  - The `@UnequipBareHand` command will remove any bare hand weapon from the actor.
  - When a dual-wielding actor has a weapon in the secondary slot, they will not receive a bare hand weapon in the primary slot.

### Example Scenarios

1. **Actor with Two Empty Slots**:
   - Primary Slot: Empty
   - Secondary Slot: Empty
   - Result: Bare hand weapon will be equipped in primary slot.

2. **Actor with Secondary Weapon**:
   - Primary Slot: Empty
   - Secondary Slot: Dagger
   - Result: No bare hand weapon will be equipped (respecting the secondary weapon).

3. **After Using @UnequipBareHand**:
   - The bare hand weapon will be removed from the actor.
   - Debug messages will confirm the unequip action.

## Examples

### Basic Unarmed Weapon Configuration

```ini
[bare_handed]
Actor1_UnarmedWeaponId=87
Actor2_UnarmedWeaponId=88
Actor3_UnarmedWeaponId=89
```

Create weapons with IDs 87, 88, and 89 in your RPG Maker 2003 database. When actors 1, 2, or 3 have no weapon equipped on the map, they will automatically equip their respective unarmed weapons.

### Variable-Based Weapon IDs

```ini
[bare_handed]
Actor1_UnarmedWeaponId=87
Actor2_VariableId=10
```

Create weapon ID 87 in your database.

- Actor 1 will always equip weapon ID 87 when unarmed.
- Actor 2's unarmed weapon will be determined by the value in variable 10.
- If variable 10 contains 50, actor 2 will equip weapon ID 50 when unarmed.
- If variable 10 contains 0 or a negative number, actor 2 won't equip any unarmed weapon.

This allows for dynamic weapon configuration during gameplay.

## Debugging

For troubleshooting, enable the debug option:

```ini
[bare_handed]
EnableDebugConfig=true  ; For debugging configuration loading
EnableDebugRuntime=true ; For debugging weapon equipping/unequipping
```

This will show message boxes when:
1. The plugin is initialized.
2. Unarmed weapons are equipped on the map.
3. Unarmed weapons are unequipped in menus.
4. Variable-controlled weapons are forcibly unequipped due to permission changes.

The debug messages include detailed information about actor IDs, weapon IDs, and variable values.

## Complete Example Configuration

```ini
[bare_handed]
; Debug options
EnableDebugConfig=false  ; For configuration loading
EnableDebugRuntime=false ; For gameplay operations

; Maximum actor ID to check
MaxActorId=20

; Actor-specific unarmed weapons
Actor1_UnarmedWeaponId=87
Actor2_UnarmedWeaponId=88
Actor3_UnarmedWeaponId=89
Actor4_UnarmedWeaponId=90

; Variable-based weapon IDs
Actor4_VariableId=10   ; Weapon ID from variable 10
Actor5_VariableId=11   ; Weapon ID from variable 11
```

## Usage

1. Configure your actor-to-weapon mappings in `DynRPG.ini` as shown above.
2. Create the corresponding weapons in your RPG Maker 2003 database.
3. The plugin will automatically manage equipping and unequipping of unarmed weapons during gameplay.
4. Use the event comment commands (see below) for additional control over bare hand weapons in events.

## Event Comment Commands

This plugin provides two special event commands that can be used in comment event commands:

### 1. Unequip Bare Hand Weapon

```
@UnequipBareHand actorId
```

This command unequips a bare hand weapon from the specified actor, but only if the currently equipped weapon is a bare hand weapon (either from fixed configuration or variable-based configuration).

- `actorId`: The ID of the actor to unequip. DynRPG automatically handles variable references when you use the `V` prefix (e.g., `V5` will use the value stored in variable 5).

This command will NOT unequip regular weapons, only bare hand weapons configured in the plugin. This ensures you don't accidentally remove weapons that the player has manually equipped.

**Example:**
```
@UnequipBareHand 1    ; Unequip bare hand weapon from actor 1
@UnequipBareHand V10  ; Unequip bare hand weapon from actor whose ID is in variable 10
```

**Note:** When using the `V` prefix, DynRPG automatically retrieves the value from the variable, so the plugin already receives the final number value.

### 2. Update Bare Hand Equipment

```
@UpdateBareHand
```

This command forces the plugin to re-check all actors' equipment and apply bare hand weapons where appropriate. Use this after unequipping weapons via events to ensure bare hand weapons are properly equipped.

**Usage scenarios:**
- After using event commands to unequip a weapon.
- After changing party members.
- When you want to force bare hand weapons to be applied without waiting for a scene change.

**Example:**
```
@UpdateBareHand  ; Force bare hand equipment update
```

### Important: Using the Change Equipment Event Command

**When UNEQUIPPING a weapon in events:**

You MUST place comment commands before and after the Change Equipment event command to prevent issues:

```
@UnequipBareHand 1    <- First remove the bare hand weapon
[Change Equipment: Actor 1, Weapon, None]  <- Then use the event command to unequip
@UpdateBareHand       <- Finally update equipment to reapply if needed
```

**When EQUIPPING a weapon in events:**

You MUST place a comment command before the Change Equipment event command:

```
@UnequipBareHand 1    <- First remove the bare hand weapon
[Change Equipment: Actor 1, Weapon, Steel Sword]  <- Then equip the new weapon
```

**Why this is necessary:**
Without these comment commands, the hidden bare hand weapons (which should not be accessible to players) may appear in the player's inventory when equipping/unequipping through events.

## Troubleshooting

If you encounter issues:

- Check that the `bare_handed.dll` file is located in the DynPlugins folder.
- Verify your `DynRPG.ini` configuration for syntax errors.
- Enable `EnableDebugConfig=true` to see detailed information about configuration loading.
- Enable `EnableDebugRuntime=true` to see detailed information about runtime operations.

## Credits

Special thanks to:

- Cherry (David Trapp) for [DynRPG](https://rpg-maker.cherrytree.at/dynrpg/).
- rewrking aka PepsiOtaku for [DynRPG](https://github.com/rewrking/DynRPG). 
