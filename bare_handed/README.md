# BareHanded Plugin for RPG Maker 2003 (DynRPG)

This DynRPG plugin allows actors without equipped weapons to automatically equip a configurable "unarmed" weapon in RPG Maker 2003, giving them proper stats for barehanded combat.

## Features

- Configurable through DynRPG.ini
- Per-actor unarmed weapon IDs
- Variable-based unarmed weapon IDs
- Automatic equipping and unequipping of unarmed weapons
- Detailed debug output in console window
- Comprehensive dual-wielding support

## Installation

1. Place `bare_handed.dll` in your game's DynPlugins folder
2. Configure the plugin in `DynRPG.ini` (see Configuration section)

## Technical Details

### Value Ranges and Limitations
- Actor IDs: Must match database IDs
- Weapon IDs: Must be positive integers
- Variable IDs: Must be positive integers
- MaxActorId: Default is 20, can be increased as needed

### MaxActorId Setting
The `MaxActorId` setting determines the highest actor ID the plugin will check for configuration:
- Default is 20 if not specified
- Increase this value if your game uses actor IDs higher than 20
- Example: `MaxActorId=30` if you have actors with IDs up to 30

### Error Handling
- Invalid weapon IDs are ignored during configuration
- Invalid variable values prevent weapon equipping
- Zero or negative values are treated as unconfigured
- Dual-wielding conflicts are automatically handled

### Debug Output Format
When debug options are enabled, the console window shows:

**Configuration Loading (EnableDebugConfig=true)**:
```
[BareHanded - Configuration]
Loaded mapping:
Actor ID: 1
Weapon ID: 87

[BareHanded - Configuration]
Variable-based mapping:
Actor ID: 4
Variable ID: 10
```

**Runtime Actions (EnableDebugRuntime=true)**:
```
[BareHanded - Debug Info]
Equipping bare hand weapon:
Actor ID: 1
Weapon ID: 87

[BareHanded - Debug Info]
Variable 10 contains invalid value: 0
Actor 4 will remain unarmed
```

## Configuration

Configure the plugin in `DynRPG.ini` using the following format:

```ini
[bare_handed]
; Debug Console Options
EnableConsole=false      ; Enable/disable debug console window
EnableDebugConfig=false  ; Show configuration loading details
EnableDebugRuntime=false ; Show runtime action details

; Maximum Actor ID Setting
MaxActorId=20           ; Default is 20 if not specified

; Direct Weapon Mapping
Actor1_UnarmedWeaponId=87   ; Actor 1 equips weapon 87 when unarmed
Actor2_UnarmedWeaponId=88   ; Actor 2 equips weapon 88 when unarmed

; Variable-Based Mapping
Actor4_VariableId=10        ; Use weapon ID from variable 10
```

### Debug Options

- `EnableConsole`: Controls the debug console window
  - false = No console window (default)
  - true = Show console window for debug output
  - Required for any debug output to be visible

- `EnableDebugConfig`: Shows configuration loading details
  - false = No configuration debug output (default)
  - true = Shows in console window:
    - Loaded actor mappings
    - Variable-based configurations
    - Invalid configurations
    - Configuration errors

- `EnableDebugRuntime`: Shows runtime action details
  - false = No runtime debug output (default)
  - true = Shows in console window:
    - Weapon equipping/unequipping
    - Variable value issues
    - Dual-wielding handling
    - Event command processing

### Actor Weapon Mappings

#### Direct Weapon Mapping
```ini
[bare_handed]
Actor1_UnarmedWeaponId=87   ; Actor 1 equips weapon 87 when unarmed
```

- Actor ID must exist in database
- Weapon ID must be positive
- Invalid mappings are ignored with debug message

#### Variable-Based Mapping
```ini
[bare_handed]
Actor4_VariableId=10        ; Use weapon ID from variable 10
```

- Variable ID must be positive
- Variable value must be positive
- Zero or negative values prevent equipping
- Invalid values are logged if debug enabled

### Error Handling Examples

1. **Invalid Weapon ID**:
```ini
Actor1_UnarmedWeaponId=0    ; Ignored: Weapon ID must be positive
Actor2_UnarmedWeaponId=-1   ; Ignored: Weapon ID must be positive
```

2. **Invalid Variable Value**:
```ini
Actor4_VariableId=10        ; Variable contains 0
; Result: Actor 4 remains unarmed
```

3. **Dual-Wielding Conflict**:
```ini
; Actor 1 has Two Wpn. flag and secondary weapon:
Actor1_UnarmedWeaponId=87   ; Not equipped: Secondary slot occupied
```

### Debug Output Examples

1. **Configuration Loading**:
```
[BareHanded - Configuration]
Loaded mapping:
Actor ID: 1
Weapon ID: 87

[BareHanded - Configuration]
Skipped mapping:
Actor ID: 2
Reason: Invalid weapon ID (0)
```

2. **Runtime Actions**:
```
[BareHanded - Debug Info]
Equipping bare hand weapon:
Actor ID: 1
Weapon ID: 87

[BareHanded - Debug Info]
Dual-wielding detected:
Actor ID: 1
Secondary slot occupied
Skipping bare hand equip
```

## Event Comment Commands

The plugin provides two special event commands that can be used in comment event commands:

### 1. Unequip Bare Hand Weapon
```
@UnequipBareHand actorId
```

This command unequips a bare hand weapon from the specified actor, but only if the currently equipped weapon is a bare hand weapon (either from fixed configuration or variable-based configuration).

- `actorId`: The ID of the actor to unequip. DynRPG automatically handles variable references when you use the `V` prefix (e.g., `V5` will use the value stored in variable 5)

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
- After using event commands to unequip a weapon
- After changing party members
- When you want to force bare hand weapons to be applied without waiting for a scene change

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
Without these comment commands, the hidden bare hand weapons (which should not be accessible to players) may appear in the player's inventory when equipping/unequipping through events. The sequence ensures that:
1. Any bare hand weapon is properly removed before equipment changes
2. The game's equipment command executes normally
3. The plugin can reapply bare hand weapons if needed (when unequipping)

## Troubleshooting

1. **Weapons Not Equipping**:
   - Enable debug output to trace actions
   - Verify weapon IDs are positive
   - Check actor exists in database
   - Confirm dual-wielding status

2. **Variable Weapons Not Working**:
   - Verify variable contains valid ID
   - Check variable ID is positive
   - Enable debug to see variable values

3. **Debug Output Not Showing**:
   - Verify EnableConsole=true
   - Check specific debug option enabled
   - Confirm bare_handed.dll location

## Credits

Special thanks to:
- Cherry (David Trapp) for [DynRPG](https://rpg-maker.cherrytree.at/dynrpg/)
- rewrking aka PepsiOtaku for [DynRPG](https://github.com/rewrking/DynRPG/)
