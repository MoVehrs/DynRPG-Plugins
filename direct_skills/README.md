# DirectSkills Plugin for RPG Maker 2003 (DynRPG)

This DynRPG plugin allows battle commands with attack archtype to automatically execute skills instead of regular attacks in RPG Maker 2003.

## Features

- Configurable through DynRPG.ini
- Direct mapping of battle commands to skills
- Variable-based skill IDs for dynamic execution
- Default skill fallback for invalid variable values
- Automatic limit_break plugin conflict detection
- Detailed debug output in console window

## Installation

1. Place `direct_skills.dll` in your game's DynPlugins folder
2. Configure the plugin in `DynRPG.ini` (see Configuration section)

## Technical Details

### Value Ranges and Limitations
- Battle command IDs: 1-100
- Battle command indices in battle: 0-3
- Skill IDs: Must be positive integers
- Variable IDs: Must be positive integers
- Default skill IDs: Must be positive integers

### Important Requirements
- Battle commands must be set to Attack archtype in the database
- Command IDs must correspond to existing battle commands
- Skills must exist in the database and be usable by the actors
- Invalid configurations will be skipped with debug messages

### Error Handling
- Invalid command IDs are skipped during configuration
- Invalid skill IDs are skipped during configuration
- Invalid variable values trigger default skill usage in battle
- Missing default skills prevent variable mappings from loading
- Conflicts with limit_break plugin are automatically detected and skipped

### Debug Output Format
When debug options are enabled, the console window shows:

**Configuration Loading (EnableDebugConfig=true)**:
```
[DirectSkills - Configuration]
Loaded mapping:
Battle Command ID: 14
Fixed Skill ID: 123

[DirectSkills - Conflict Detection]
Conflict detected: BattleCommandId15 is also used as LimitCommandId
This mapping will be skipped to avoid conflicts.
```

**Battle Actions (EnableDebugBattle=true)**:
```
[DirectSkills - Debug Info]
Action Swapped for Actor1
Selected Command Index: 2
Battle Command ID: 14 (Attack)
Skill ID: 123

[DirectSkills - Debug Info]
Variable 45 contains invalid value: 0
Using default skill ID: 150
```

## Configuration

Configure the plugin in `DynRPG.ini` using the following format:

```ini
[direct_skills]
; Debug Console Options
EnableConsole=false      ; Enable/disable debug console window
EnableDebugConfig=false  ; Show configuration loading details
EnableDebugBattle=false  ; Show battle action details

; Direct Skill Mapping
BattleCommandId14=123   ; Command 14 executes skill 123

; Variable-Based Mapping
BattleCommandId15=v45                ; Use skill ID from variable 45
BattleCommandId15_DefaultId=150      ; Fallback if variable 45 is invalid
```

### Debug Options

- `EnableConsole`: Controls the debug console window
  - false = No console window (default)
  - true = Show console window for debug output
  - Required for any debug output to be visible

- `EnableDebugConfig`: Shows configuration loading details
  - false = No configuration debug output (default)
  - true = Shows in console window:
    - Loaded command mappings
    - Skipped/invalid mappings
    - Limit break plugin conflicts
    - Configuration errors

- `EnableDebugBattle`: Shows battle action details
  - false = No battle debug output (default)
  - true = Shows in console window:
    - Selected command details
    - Skill replacements
    - Variable value issues
    - Default skill usage

### Battle Command Mappings

#### Direct Skill Mapping
```ini
[direct_skills]
BattleCommandId14=123   ; Command 14 executes skill 123
```

- Command ID must be between 1-100
- Skill ID must be a positive integer
- Invalid mappings are skipped with debug message

#### Variable-Based Mapping
```ini
[direct_skills]
BattleCommandId15=v45                ; Use skill ID from variable 45
BattleCommandId15_DefaultId=150      ; Fallback skill ID
```

- Variable ID must be positive
- Default skill ID must be positive
- Default skill is required for variable mappings
- Invalid variable values trigger default skill usage

### Error Handling Examples

1. **Invalid Command ID**:
```ini
BattleCommandId0=123    ; Skipped: Command ID < 1
BattleCommandId101=123  ; Skipped: Command ID > 100
```

2. **Invalid Skill ID**:
```ini
BattleCommandId14=0     ; Skipped: Skill ID must be positive
BattleCommandId15=-1    ; Skipped: Skill ID must be positive
```

3. **Missing Default Skill**:
```ini
BattleCommandId16=v50   ; Skipped: No default skill specified
```

4. **Limit Break Conflict**:
```ini
; If limit_break plugin uses command ID 20:
BattleCommandId20=123   ; Skipped: Conflicts with limit_break
```

### Debug Output Examples

1. **Configuration Loading**:
```
[DirectSkills - Configuration]
Loaded mapping:
Battle Command ID: 14
Fixed Skill ID: 123

[DirectSkills - Configuration]
Skipped mapping:
Battle Command ID: 16
Reason: Missing default skill ID for variable mapping
```

2. **Battle Actions**:
```
[DirectSkills - Debug Info]
Action Swapped for Actor1
Selected Command Index: 2
Battle Command ID: 14 (Attack)
Skill ID: 123

[DirectSkills - Debug Info]
Variable 45 contains invalid value: -1
Using default skill ID: 150
```

## Compatibility with limit_break Plugin

The plugin automatically handles conflicts with the limit_break plugin:

1. Detects limit_break configuration in DynRPG.ini
2. Identifies command IDs used by limit_break:
   - Regular limit break command
   - Ultimate limit break command
3. Skips any conflicting mappings
4. Outputs conflict details when EnableDebugConfig=true

Example conflict detection:
```
[DirectSkills - Conflict Detection]
Conflict detected: BattleCommandId15 is also used as LimitCommandId
This mapping will be skipped to avoid conflicts.
```

## Troubleshooting

1. **Command Not Working**:
   - Verify command ID is between 1-100
   - Check command has Attack archtype
   - Enable debug output to see if mapping was loaded

2. **Variable Skill Not Working**:
   - Verify variable contains valid skill ID
   - Check default skill ID is configured
   - Enable debug output to see variable values

3. **Debug Output Not Showing**:
   - Verify EnableConsole=true
   - Check specific debug option is enabled
   - Confirm direct_skills.dll is in DynPlugins folder

## Credits

Special thanks to:
- Cherry (David Trapp) for [DynRPG](https://rpg-maker.cherrytree.at/dynrpg/)
- rewrking aka PepsiOtaku for [DynRPG](https://github.com/rewrking/DynRPG)
