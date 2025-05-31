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
1. Allowing memory modifications to be changed dynamically during gameplay
2. Mapping RPG Maker variables to specific memory addresses
3. Automatically updating memory when variables change
4. Ensuring memory is updated correctly when loading saved games and reset when starting a new game

## Features

- Map RPG Maker variables to specific memory addresses (format: 0x followed by 6 hex digits)
- Support for three value formats:
  - 8-bit signed integers (-127 to 127, with value clamping)
  - 32-bit signed integers
  - Raw hexadecimal values
- Automatic memory updates when variable values change
- Configurable automatic updates when loading saved games
- Automatic restoration of original memory values
- Configurable variable ID range
- Comprehensive memory safety features
- Detailed debug output system
- Up to 100 QuickPatch mappings supported

## Installation

1. Place the `dynamic_quickpatch.dll` file in your game's DynPlugins folder
2. Configure the plugin in your `DynRPG.ini` file (see Configuration section)

## Technical Details

### Value Types and Ranges

1. **8-bit Values (8bit or % type)**
   - Range: -127 to 127
   - Values outside range are automatically clamped
   - Example: Variable value 200 becomes 127

2. **32-bit Values (32bit or # type)**
   - Full 32-bit signed integer range
   - No value clamping applied
   - Direct value mapping

3. **Hex Values (hex type)**
   - Must be even length (complete byte pairs)
   - Variable acts as on/off switch (0 = off, >0 = on)
   - Original memory restored when disabled

### Memory Safety Features

1. **Address Requirements**
   - Must use format: 0x followed by exactly 6 hex digits
   - Example: 0x401234 (correct), 0x00401234 (incorrect)
   - Decimal addresses are not supported
   - Rejects address 0x000000
   - Rejects addresses near 0xFFFFFF
   - Validates length for multi-byte operations

2. **Original Value Storage**
   - Stores original memory before patching
   - Restores original values when:
     - Patch is disabled (variable = 0)
     - New game is started
     - Returning to title screen

3. **Error Handling**
   - Invalid addresses are rejected
   - Memory access violations are caught
   - Configuration errors are logged
   - Invalid hex strings are rejected

### Debug System

1. **Console Window**
   - Optional debug console (EnableConsole setting)
   - Multiple message categories:
     - [DynamicQuickPatch - Configuration]
     - [DynamicQuickPatch - Memory]
     - [DynamicQuickPatch - Memory Error]
     - [DynamicQuickPatch - Range Warning]
     - [DynamicQuickPatch - Patch Disabled]
     - [DynamicQuickPatch - Memory Update]
     - [DynamicQuickPatch - Load Game]

2. **Debug Output Format**
   ```
   [DynamicQuickPatch - Category]
   Action or Status Message
   Relevant Details...
   Additional Information...
   ```

## Configuration

Configure the plugin in `DynRPG.ini` using the following format:

```ini
[dynamic_quickpatch]
; Debug Console Setting
EnableConsole=true|false

; Maximum Variable ID
MaxVariableId=NUMBER

; QuickPatch Mapping Pattern (up to 100 entries)
QuickPatchN_VariableId=VARIABLE_ID
QuickPatchN_Address=MEMORY_ADDRESS (must use 0x prefix and 6 digits)
QuickPatchN_Type=TYPE
QuickPatchN_HexValue=HEX_STRING
QuickPatchN_OnLoadGame=true|false
```

### Working with Hex Values

When using the `hex` type, the `HexValue` is static and cannot directly use the variable's value. RPG Maker 2003 variables can only store integers, not hex strings. Here's how to use hex values effectively:

#### Using Variables as Activation Switches

For hex values, the variable acts as an on/off switch:
- When variable = 0: The patch is OFF (original memory values remain)
- When variable > 0: The patch is ON (hex value is applied)

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

#### Converting Between Decimal and Hex

Remember these conversions when working with hex values:

- Decimal 235 = Hex 0xEB (Jump instruction used in HideEXP)
- Decimal 144 = Hex 0x90 (NOP instruction)
- Decimal 232 = Hex 0xE8 (CALL instruction)

In RPG Maker events, you might use a script like:
```
@>Variable [005:HideEXP] = 1     // Activate HideEXP
@>Variable [006:JumpByte] = 235  // Sets value to EB in hex (JMP instruction)
@>Variable [007:WindowX] = 144   // Sets window X coordinate to 144
```

### Configuration Parameters

1. **EnableConsole**
   - Controls debug console window
   - Supported values:
     - Enable: true, 1, yes, y, on
     - Disable: false, 0, no, n, off (default)

2. **MaxVariableId**
   - Upper limit for variable IDs (default: 1000)
   - Must be positive integer
   - Values <= 0 use default of 1000

3. **QuickPatch Entries**
   - Maximum 100 entries (QuickPatch1 through QuickPatch100)
   - Required fields:
     - VariableId: 1 to MaxVariableId
     - Address: Must use 0x prefix and 6 digits (e.g., 0x401234)
     - Type: 8bit, 32bit, or hex
   - Optional fields:
     - HexValue: Required only for hex type
     - OnLoadGame: Defaults to true if omitted

### Configuration Examples

1. **8-bit Value Mapping**
```ini
QuickPatch1_VariableId=1
QuickPatch1_Address=0x401234
QuickPatch1_Type=8bit
QuickPatch1_OnLoadGame=true
```

2. **32-bit Value Mapping**
```ini
QuickPatch2_VariableId=2
QuickPatch2_Address=0x40ABCD
QuickPatch2_Type=32bit
QuickPatch2_OnLoadGame=false
```

3. **Hex Value Mapping**
```ini
QuickPatch3_VariableId=3
QuickPatch3_Address=0x410000
QuickPatch3_Type=hex
QuickPatch3_HexValue=90ABCD
```

4. **Multiple Hex Patches (Same Variable)**
```ini
; Both controlled by Variable #5
QuickPatch5_VariableId=5
QuickPatch5_Address=0x49E148
QuickPatch5_Type=hex
QuickPatch5_HexValue=EB71

QuickPatch6_VariableId=5
QuickPatch6_Address=0x49F1CA
QuickPatch6_Type=hex
QuickPatch6_HexValue=EB67
```

## Finding Memory Addresses

You can find memory addresses to modify in several ways:

1. Look at existing quickpatches for inspiration
2. Use a memory editor like Cheat Engine to locate values you want to patch
3. Use a debugger like OllyDbg or x64dbg to step through the program and find where values are set or accessed
4. Look at a disassembly using tools like Ghidra or IDA to find functions and static addresses of interest

Once you've found a memory address:
1. Add it to your configuration
2. Test with small changes to ensure you've found the correct address

## Example Applications

- Dynamically changing window positions based on game events
- Creating adjustable graphic effects by modifying render parameters
- Implementing difficulty settings that affect game mechanics at the binary level
- Creating visual hacks that can be toggled on/off during gameplay

## Troubleshooting

1. **Patch Not Working**
   - Verify address format (0x######)
   - Check variable ID range
   - Verify address validity
   - Check value ranges

2. **Memory Errors**
   - Verify address has exactly 6 digits
   - Check for access violations
   - Monitor debug output
   - Verify patch type

3. **Debug Output Issues**
   - Verify EnableConsole=true
   - Check console initialization
   - Monitor error messages
   - Check configuration loading

## Credits

Special thanks to:
- Cherry (David Trapp) for [DynRPG](https://rpg-maker.cherrytree.at/dynrpg/)
- rewrking aka PepsiOtaku for [DynRPG](https://github.com/rewrking/DynRPG)
