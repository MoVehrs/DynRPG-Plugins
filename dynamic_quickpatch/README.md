# DynamicQuickPatch Plugin for RPG Maker 2003 (DynRPG)

This DynRPG plugin allows you to map RPG Maker 2003 variables to memory addresses, enabling dynamic modification of memory values during gameplay. It extends the concept of DynRPG's static quickpatches into the runtime environment, giving you complete control over memory modifications while your game is running.

## About Quickpatches vs DynamicQuickPatch

DynRPG's standard quickpatches are static memory modifications defined in the DynRPG.ini file that are applied once when the game loads:

```ini
[QuickPatches]
; Hides EXP in main menu and save screen
HideEXP=49E148,EB71,49F1CA,EB67,49F095,EB21
; Moves the window on the title screen to coordinates 144/56
MoveTitleWindow=490821,#144,490828,#56
```

This plugin takes the concept further by:
1. Allowing memory modifications to be changed dynamically during gameplay.
2. Mapping RPG Maker variables to specific memory addresses.
3. Automatically updating memory when variables change.
4. Ensuring memory is updated correctly when loading saved games and reset when starting a new game.

## Features

- Map RPG Maker variables to specific memory addresses.
- Support for three value formats (same as quickpatches):
  - 8-bit signed integers (% notation).
  - 32-bit signed integers (# notation).
  - Raw hexadecimal values.
- Automatic memory updates when variable values change.
- Configurable automatic updates when loading saved games (can be enabled/disabled per patch).
- Automatic restoration of original memory values when starting a new game.
- Configurable variable ID range to match your game's database.
- Range validation to prevent invalid memory writes.
- Optional debug messages showing memory changes.

## Installation

1. Place the `dynamic_quickpatch.dll` file in your game's DynPlugins folder.
2. Configure the plugin in your `DynRPG.ini` file (see below).

## Configuration

Configure the plugin in `DynRPG.ini` using the following format:

```ini
[dynamic_quickpatch]
; Enable or disable debug message boxes
ShowDebugMessages=true|false

; Maximum variable ID (default: 1000)
MaxVariableId=NUMBER

; QuickPatch mapping pattern:
QuickPatchN_VariableId=VARIABLE_ID
QuickPatchN_Address=MEMORY_ADDRESS
QuickPatchN_Type=TYPE
QuickPatchN_HexValue=HEX_STRING (only for hex type)
QuickPatchN_OnLoadGame=true|false (optional)
```

### Configuration Parameters

- `ShowDebugMessages`: Set to `true` to enable debug message boxes showing all memory changes, or `false` to disable them.
- `MaxVariableId`: The maximum variable ID to allow in your configuration (default: 1000). Set this to match the maximum variable ID in your game's database.
- `QuickPatchN_VariableId`: The RPG Maker variable ID (1 to MaxVariableId) that will control this memory address.
- `QuickPatchN_Address`: The memory address to patch, in hexadecimal format with `0x` prefix (e.g., `0x496AC7`).
- `QuickPatchN_Type`: The type of value to write:
  - `8bit` (or `%`): 8-bit signed integer (-127 to 127).
  - `32bit` (or `#`): 32-bit signed integer.
  - `hex`: Raw hexadecimal bytes.
- `QuickPatchN_HexValue`: For `hex` type only, the hexadecimal string to write (e.g., `90ABCD`).
- `QuickPatchN_OnLoadGame`: Optional setting to control whether this patch should be applied when loading a save game. Set to `true` to apply (default) or `false` to skip this patch when loading a save.

Replace `N` with sequential numbers starting from 1 for each mapping.

### Working with Hex Values

When using the `hex` type, the `HexValue` is static and cannot directly use the variable's value. RPG Maker 2003 variables can only store integers, not hex strings. Here's how to use hex values effectively:

#### Using Variables as Activation Switches

For hex values, the variable acts as an on/off switch:
- When variable = 0: The patch is OFF (original memory values remain).
- When variable > 0: The patch is ON (hex value is applied).

Here's an example using the "HideEXP" quickpatch components:

```ini
; Variable 5 controls the EXP display in menus:
; Value 0 = Show EXP (original behavior, patch inactive)
; Value 1+ = Hide EXP (patch active, using the HideEXP quickpatch values)

; First component: Modifies code at 0x49E148
QuickPatch1_VariableId=5
QuickPatch1_Address=0x49E148
QuickPatch1_Type=hex
QuickPatch1_HexValue=EB71

; Second component: Modifies code at 0x49F1CA
QuickPatch2_VariableId=5
QuickPatch2_Address=0x49F1CA
QuickPatch2_Type=hex
QuickPatch2_HexValue=EB67

; Third component: Modifies code at 0x49F095
QuickPatch3_VariableId=5
QuickPatch3_Address=0x49F095
QuickPatch3_Type=hex
QuickPatch3_HexValue=EB21
```

Then in your game, set Variable #5 to 0 to show EXP (deactivate patches), or any value greater than 0 to hide EXP (activate patches).

#### Alternative: Using 8-bit or 32-bit Values for Dynamic Control

If you need direct numeric control over memory values, use the 8-bit or 32-bit types instead of hex:

```ini
; Variable 6 controls a single byte at 0x49E148
; Setting it to 235 (0xEB in hex) would apply the jump instruction
; used in the HideEXP quickpatch
QuickPatch4_VariableId=6
QuickPatch4_Address=0x49E148
QuickPatch4_Type=8bit

; Variable 7 controls window X position (example using 32-bit value)
QuickPatch5_VariableId=7
QuickPatch5_Address=0x490821
QuickPatch5_Type=32bit
```

For the 8-bit and 32-bit types, the actual value of the variable is used to modify memory.

#### Converting Between Decimal and Hex

Remember these conversions when working with hex values:

- Decimal 235 = Hex 0xEB (Jump instruction used in HideEXP).
- Decimal 144 = Hex 0x90 (NOP instruction).
- Decimal 232 = Hex 0xE8 (CALL instruction).

In RPG Maker events, you might use a script like:
```
@>Variable [005:HideEXP] = 1     // Activate HideEXP
@>Variable [006:JumpByte] = 235  // Sets value to EB in hex (JMP instruction)
@>Variable [007:WindowX] = 144   // Sets window X coordinate to 144
```

## Example Configuration

```ini
[dynamic_quickpatch]
; Enable debug message boxes
ShowDebugMessages=true

; Maximum variable ID (default: 1000)
; Set this to match the maximum variable ID in your game's database
MaxVariableId=2000

; QuickPatch mapping #1 - Example of 8-bit value mapping
QuickPatch1_VariableId=1
QuickPatch1_Address=0x401234
QuickPatch1_Type=8bit
QuickPatch1_OnLoadGame=true

; QuickPatch mapping #2 - Example of 32-bit value mapping
; This patch will NOT be applied when loading a save game
QuickPatch2_VariableId=2
QuickPatch2_Address=0x40ABCD
QuickPatch2_Type=32bit
QuickPatch2_OnLoadGame=false

; QuickPatch mapping #3 - Example of raw hex value mapping
QuickPatch3_VariableId=3
QuickPatch3_Address=0x410000
QuickPatch3_Type=hex
QuickPatch3_HexValue=90ABCD
; OnLoadGame defaults to true when omitted
```

## Usage

1. Configure your memory mappings in `DynRPG.ini` as shown above.
2. Set the `MaxVariableId` to match your game's database (default is 1000 if not specified).
3. In your RPG Maker 2003 game, use the `Change Variable` command to set the value of the mapped variables.
4. The plugin will automatically update the corresponding memory addresses.
5. When loading a saved game, the plugin will automatically update memory addresses based on the current variable values, but only for patches that have `OnLoadGame=true` (or where this setting is omitted, as it defaults to true).
6. When starting a new game, the plugin will automatically restore all memory locations to their original values.

## Finding Memory Addresses

You can find memory addresses to modify in several ways:

1. Look at existing quickpatches for inspiration.
2. Use a memory editor like Cheat Engine to locate values you want to patch.
3. Use a debugger like OllyDbg or x64dbg to step through the program and find where values are set or accessed.
4. Look at a disassembly using tools like Ghidra or IDA to find functions and static addresses of interest.

Once you've found a memory address:
1. Add it to your configuration.
2. Test with small changes to ensure you've found the correct address.

## Example Applications

- Dynamically changing window positions based on game events.
- Creating adjustable graphic effects by modifying render parameters.
- Implementing difficulty settings that affect game mechanics at the binary level.
- Creating visual hacks that can be toggled on/off during gameplay.

## Important Notes

- Be careful when modifying memory addresses, as incorrect values can cause the game to crash.
- Memory addresses are specific to each version of RPG Maker 2003 and may vary.
- The plugin performs bounds checking for 8-bit values (-127 to 127) and automatically clamps values outside this range (values below -127 are clamped to -127, values above 127 are clamped to 127).

## Troubleshooting

If you encounter issues:

- Check that the `dynamic_quickpatch.dll` file is located in the DynPlugins folder.
- Verify your `DynRPG.ini` configuration for syntax errors.
- Enable `ShowDebugMessages=true` to see detailed information about memory changes.
- Ensure variable IDs are within the valid range (1-1000).
- For hex type patches, ensure the hex string has an even number of characters.

## Credits

Special thanks to:

- Cherry (David Trapp) for [DynRPG](https://rpg-maker.cherrytree.at/dynrpg/).
- rewrking aka PepsiOtaku for [DynRPG](https://github.com/rewrking/DynRPG). 
