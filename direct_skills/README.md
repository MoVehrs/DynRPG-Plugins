# Direct Skills System Plugin for RPG Maker 2003 (DynRPG)

This DynRPG plugin allows battle commands with attack archtype to automatically execute skills instead of regular attacks in RPG Maker 2003.

## Features

- Fully configurable through DynRPG.ini.
- Direct mapping of battle commands to skills.
- Support for variable-based skill IDs for dynamic skill execution.
- Default skill ID fallback for variable-based mappings.
- Compatible with the limit_break plugin (conflict detection).
- Separate debug options for configuration and battle action logging.

## Installation

1. Place the `direct_skills.dll` file in your game's DynPlugins folder.
2. Configure the plugin in your `DynRPG.ini` file (see below).

## Configuration

Configure the plugin in `DynRPG.ini` using the following format:

```ini
[direct_skills]
; (OPTIONAL) Enables or disables debug message boxes for configuration loading
; false = No debug messages for configuration (default)
; true = Show detailed message boxes when loading mappings from DynRPG.ini
EnableDebugConfig=false

; (OPTIONAL) Enables or disables debug message boxes for battle actions
; false = No debug messages for battle actions (default)
; true = Show detailed message boxes when battle commands are swapped to skills
EnableDebugBattle=false

; Battle command to skill mappings
; Format: BattleCommandIdX=Y
; Where X is the battle command ID from the database and Y is the skill ID to execute
; Examples:
; BattleCommandId14=123   ; When battle command 14 is used, skill 123 will be executed
; BattleCommandId15=45    ; When battle command 15 is used, skill 45 will be executed

; For variable-based skill IDs, use the 'v' prefix before the variable ID
; Format: BattleCommandIdX=vY
; Where X is the battle command ID and Y is the variable ID containing the skill ID
; Examples:
; BattleCommandId16=v50   ; When battle command 16 is used, the skill ID stored in variable 50 will be executed
; BattleCommandId17=v51   ; When battle command 17 is used, the skill ID stored in variable 51 will be executed

; (REQUIRED) For variable-based mappings, you MUST specify a default skill ID to use
; if the variable contains an invalid value (0 or negative) or is uninitialized
; Format: BattleCommandIdX_DefaultId=Y
; Where X is the battle command ID and Y is the default skill ID to use
; Examples:
; BattleCommandId16_DefaultId=200   ; (REQUIRED) Default skill ID for BattleCommandId16=v50
; BattleCommandId17_DefaultId=201   ; (REQUIRED) Default skill ID for BattleCommandId17=v51

; Add your battle command to skill mappings below:
BattleCommandId14=123
BattleCommandId15=v45
BattleCommandId15_DefaultId=150   ; Default skill ID to use if variable 45 contains an invalid value
```

### Debug Options

- `EnableDebugConfig`: Whether to display debug message boxes during configuration loading (**OPTIONAL**).
  - false = No debug messages (default).
  - true = Show detailed message boxes when loading mappings from DynRPG.ini.
  - Useful for verifying that battle commands are properly mapped to skills.

- `EnableDebugBattle`: Whether to display debug message boxes during battle (**OPTIONAL**).
  - false = No debug messages (default).
  - true = Show detailed message boxes when battle commands are swapped to skills.
  - Useful for confirming that the plugin is working correctly during gameplay.

### Battle Command to Skill Mappings

The plugin requires one or more battle command to skill mappings to function. Each mapping follows this format:

```ini
BattleCommandIdX=Y
```

Where:
- `X` is the battle command ID from your RPG Maker 2003 database.
- `Y` is the skill ID that should be executed when this command is used.

For example:
```ini
BattleCommandId14=123
```

This means that when battle command 14 is used, skill 123 will be executed instead of the default attack action.

### Variable-Based Skill IDs

For dynamic skill execution, you can use variable-based skill IDs. The format is:

```ini
BattleCommandIdX=vY
BattleCommandIdX_DefaultId=Z  ; REQUIRED for variable-based mappings
```

Where:
- `X` is the battle command ID.
- `Y` is the variable ID containing the skill ID.
- `Z` is the default skill ID to use if the variable contains an invalid value.

For example:
```ini
BattleCommandId15=v45
BattleCommandId15_DefaultId=150  ; REQUIRED for variable-based mappings
```

This means that when battle command 15 is used, the plugin will look at the current value of variable 45 and use that as the skill ID. This allows you to change which skill is executed at runtime by updating the variable value.

If the variable contains an invalid skill ID (0 or negative), the plugin will use the default skill ID specified by `BattleCommandId15_DefaultId` instead. This ensures that a valid skill is always executed, even if the variable hasn't been set properly.

**Important**: When using variable-based mappings, you MUST specify a default skill ID. If you don't, the mapping will be skipped and an error message will be displayed during configuration loading.

### Compatibility with Limit Break Plugin

The DirectSkills plugin is designed to work alongside the Limit Break plugin. It will automatically detect if the Limit Break plugin is active and avoid conflicts by:

1. Detecting if the Limit Break plugin is configured in DynRPG.ini.
2. Identifying any battle commands that are also used by the Limit Break plugin.
3. Skipping those mappings with a warning message to prevent conflicts.

For optimal compatibility, avoid using the same battle command IDs that are configured in the Limit Break plugin:
- Do not use the battle command ID specified by `LimitCommandId` in the Limit Break configuration.
- Do not use the battle command ID specified by `UltimateLimitCommandId` in the Limit Break configuration.

## How It Works

1. When a battle command is used, the plugin checks if it is configured in the DynRPG.ini.
2. If the command is found in the configuration, the plugin checks if the action is a basic attack.
3. If it is a basic attack (Attack or Double Attack), the plugin replaces it with the configured skill action.
4. The skill is executed instead of the default attack action.

## Examples

### Fixed Skill Mapping

```ini
[direct_skills]
BattleCommandId14=123
```

Create a battle command with ID 14 (e.g., name it "Special") in your RPG Maker 2003 database. When this command is used in battle, skill 123 will be executed instead of a basic attack.

### Variable-Based Skill Mapping with Required Default Fallback

```ini
[direct_skills]
BattleCommandId15=v45
BattleCommandId15_DefaultId=150  ; REQUIRED for variable-based mappings
```

Create a battle command with ID 15 (e.g., name it "Variable") in your RPG Maker 2003 database. Set variable 45 to different skill IDs (e.g., 200, 201, 202) during gameplay. When this command is used in battle, the plugin will execute whichever skill ID is currently stored in variable 45.

If variable 45 contains 0 or a negative value, the plugin will fall back to using skill ID 150 instead. This ensures that a valid skill is always executed, even if the variable hasn't been properly initialized or was set to an invalid value.

**Note**: The default skill ID is required for all variable-based mappings. If you don't provide a default skill ID, the variable mapping will be skipped with an error message during configuration loading.

### Multiple Command Mapping

```ini
[direct_skills]
BattleCommandId14=123
BattleCommandId15=v45
BattleCommandId15_DefaultId=150
BattleCommandId16=124
BattleCommandId17=125
```

Configure multiple battle commands to execute different skills. This allows you to create a variety of special attacks that can be executed directly from the command menu without going through skill selection.

## Debugging

For troubleshooting, enable both debug options:

```ini
[direct_skills]
EnableDebugConfig=true
EnableDebugBattle=true
```

This will show message boxes when:
1. Battle commands are mapped to skills during configuration loading.
2. Battle commands are converted to skill actions during battle.

The debug messages include detailed information about battle command IDs, names, and the skill IDs they are mapped to.

## Complete Example Configuration

```ini
[direct_skills]
; Debug options
EnableDebugConfig=false
EnableDebugBattle=false

; Fixed skill mappings
BattleCommandId14=123
BattleCommandId16=124
BattleCommandId17=125

; Variable-based skill mapping
BattleCommandId15=v45
BattleCommandId15_DefaultId=150
```

## Usage

1. Configure your battle command to skill mappings in `DynRPG.ini` as shown above.
2. Create the corresponding battle commands in your RPG Maker 2003 database.
3. For variable-based mappings, use the `Change Variable` command to set variable values to valid skill IDs.
4. In battle, when a player selects a mapped battle command, the corresponding skill will be executed automatically.

## Troubleshooting

If you encounter issues:

- Check that the `direct_skills.dll` file is located in the DynPlugins folder.
- Verify your `DynRPG.ini` configuration for syntax errors.
- Enable `EnableDebugConfig=true` to see detailed information about configuration loading.
- Enable `EnableDebugBattle=true` to see detailed information about battle command mappings.
- Make sure that used battle command IDs are set to Attack archtype.

## Credits

Special thanks to:

- Cherry (David Trapp) for [DynRPG](https://rpg-maker.cherrytree.at/dynrpg/).
- rewrking aka PepsiOtaku for [DynRPG](https://github.com/rewrking/DynRPG). 
