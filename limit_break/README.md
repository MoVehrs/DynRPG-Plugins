# Limit Break System Plugin for RPG Maker 2003 (DynRPG)

This DynRPG plugin implements a Final Fantasy-style limit break system for RPG Maker 2003 using DynRPG.

## Features

- Fully configurable through DynRPG.ini.
- Multiple limit gain modes for different play styles.
- Actor-specific limit skills.
- Equipment-based multipliers for limit gain.
- Support for multi-hit attacks and skills.
- Ultimate limit system for party-wide limit breaks.
- Ultimate limit command for executing powerful actor-specific skills.
- Animated ultimate limit bar with customizable frames.
- Sound effect when ultimate limit bar reaches 100%.
- Switch-controlled visibility for the ultimate bar.

## Installation

1. Place the `limit_break.dll` file in your game's DynPlugins folder.
2. Configure the plugin in your `DynRPG.ini` file (see below).
3. Create the required image files in the `DynRessource\LimitBreak` folder.

## Configuration

Configure the plugin in `DynRPG.ini` using the following format:

```ini
[limit_break]
; (OPTIONAL) Determines whether to use 3 or 4 actors for ultimate limit calculation
; false = Use 3 actors (33% each), true = Use 4 actors (25% each)
; Set this based on the maximum party size in your game
; Ultimate bar is only calculated when party is at full capacity (3 or 4 actors)
; Unconfigured actors count as 0% limit, reducing the ultimate bar's maximum potential
UseFourActorsForUltimate=false

; (REQUIRED) The ID of your "Limit" battle command in the database
; This must be set to the ID of the command you want to use for limit breaks
; This specific command ID is used for Attack->SkillID conversion by the plugin
; You can create additional Limit commands in your game (e.g., ones that open skill menus
; or subtype windows), but those should use different command IDs
; The command name can be "Limit" or any other name you prefer
LimitCommandId=12

; (OPTIONAL) Maximum actor ID to check for configuration
; Default is 20 if not specified
; Increase this value if your game uses actor IDs higher than 20
MaxActorId=20

; (OPTIONAL) Enables or disables debug message boxes during battle
; false = No debug messages (default)
; true = Show detailed message boxes for damage detection, limit gain calculations, etc.
; Useful for testing and troubleshooting, but should be disabled for release versions
EnableDebugMessages=true

; (REQUIRED) Variable ID for storing the Ultimate Limit Bar value (0-100)
; This variable will be updated automatically by the plugin
; Set to 0 to disable the Ultimate Limit system entirely
UltimateLimitVarId=30

; (OPTIONAL) The ID of your "Ultimate Limit" battle command in the database
; This command can be used when the ultimate bar reaches 100%
; When used, it will execute the actor's Ultimate Limit skill
; Set to 0 to disable this feature
UltimateLimitCommandId=13

; (OPTIONAL) Enables or disables drawing the Ultimate Limit Bar gauge on the battle screen
; true = Draw the gauge bar (default), false = Do not draw the gauge bar
DrawUltimateBar=true

; (OPTIONAL) Controls the orientation of the Ultimate Limit Bar gauge
; false = Horizontal bar (default), true = Vertical bar
; When vertical, the bar fills from bottom to top instead of left to right
UseVerticalBar=false

; (OPTIONAL) Switch ID to control ultimate bar visibility
; 0 = Always visible (default), otherwise uses the specified switch ID
; When the switch is ON, the bar is visible; when OFF, the bar is hidden
UltimateBarSwitchId=123

; (OPTIONAL) Ultimate Limit Bar gauge image and position settings
; The gauge consists of three images: background.png, bar.png, foreground.png
; Place these images in GameRoot\DynRessource\LimitBreak
; The following coordinates control the position and size of the gauge:
; UltimateBarBgX=    ; X position for background/foreground images
; UltimateBarBgY=    ; Y position for background/foreground images
; UltimateBarBarX=   ; X position for the bar image (0% fill)
; UltimateBarBarY=   ; Y position for the bar image
;                    ; For horizontal bars: This is the actual Y position of the bar
;                    ; For vertical bars: This is the TOP position of the bar container
;                    ; (the bar will fill from bottom to top within this container)
; UltimateBarWidth=  ; Width in pixels for the bar at 100% fill (horizontal mode)
; UltimateBarHeight= ; Height in pixels for the bar at 100% fill (vertical mode)
UltimateBarBgX=160
UltimateBarBgY=16
UltimateBarBarX=164
UltimateBarBarY=20
UltimateBarWidth=120
UltimateBarHeight=120

; (OPTIONAL) Sound effect settings for when the ultimate bar reaches 100%
; The sound will play once when the bar first reaches 100%
; It won't play again until the bar drops below 100% and reaches 100% again
PlaySound100Percent=true
Sound100PercentFile=jingle1
Sound100PercentVolume=100
Sound100PercentSpeed=100
Sound100PercentPan=50

; (OPTIONAL) Bar animation settings
; Set BarUseAnimation=true to enable frame-based animation for the bar
; BarFrameCount specifies how many frames are in the bar.png image (stacked vertically)
; BarAnimationSpeed controls how fast the animation plays (higher = slower)
; UnfilledFrames and FilledFrames specify which frames to use for different bar states
BarUseAnimation=true
BarFrameCount=10
BarAnimationSpeed=5
UnfilledFrames=0,1,2
FilledFrames=3,4,5,6,7,8,9

; (OPTIONAL) Background animation settings
; Set BgUseAnimation=true to enable frame-based animation for the background
; BgFrameCount specifies how many frames are in the background.png image (stacked vertically)
; BgAnimationSpeed controls how fast the animation plays (higher = slower)
; BgUnfilledFrames and BgFilledFrames specify which frames to use for different bar states
BgUseAnimation=true
BgFrameCount=4
BgAnimationSpeed=10
BgUnfilledFrames=0
BgFilledFrames=1,2,3

; (OPTIONAL) Foreground animation settings
; Set FgUseAnimation=true to enable frame-based animation for the foreground
; FgFrameCount specifies how many frames are in the foreground.png image (stacked vertically)
; FgAnimationSpeed controls how fast the animation plays (higher = slower)
; FgUnfilledFrames and FgFilledFrames specify which frames to use for different bar states
FgUseAnimation=true
FgFrameCount=3
FgAnimationSpeed=8
FgUnfilledFrames=0
FgFilledFrames=1,2

; Actor configurations - Each actor requires all five settings below
; IMPORTANT: Actor numbers refer to database IDs, not battle positions
; For example, Actor1 settings apply to the actor with ID 1 in the database,
; regardless of their position in battle
; Actors not configured here will be ignored by the limit break system
; You can configure as many or as few actors as you want (including none)

; Actor 1 configuration
Actor1LimitVarID=10
Actor1ModeVarID=21
Actor1DefaultMode=0
Actor1LimitSkillVarID=31
Actor1DefaultLimitSkillID=125
Actor1UltimateLimitSkillID=150

; Actor 2 configuration
Actor2LimitVarID=11
Actor2ModeVarID=22
Actor2DefaultMode=1
Actor2LimitSkillVarID=32
Actor2DefaultLimitSkillID=51
Actor2UltimateLimitSkillID=151

; Actor 3 configuration
Actor3LimitVarID=12
Actor3ModeVarID=23
Actor3DefaultMode=2
Actor3LimitSkillVarID=33
Actor3DefaultLimitSkillID=52
Actor2UltimateLimitSkillID=152

; Equipment multipliers - Format: EquipXXX=multiplier (where XXX is the equipment ID)
; These values are added to the base multiplier of 1.0
; Equipment multipliers are optional
Equip54=1.0
Equip55=0.5
Equip56=-1.0
```

### Basic Settings

- `LimitCommandId`: The ID of your "Limit" battle command (**REQUIRED**).
- `UltimateLimitVarId`: Variable ID for storing the Ultimate Limit Bar value (0-100) (**REQUIRED**).
  - Set to 0 to disable the Ultimate Limit system entirely.
  - When enabled (value > 0), this variable will be automatically updated by the plugin.
- `UseFourActorsForUltimate`: Whether to use 4 actors (true) or 3 actors (false) for the Ultimate Limit calculation (**OPTIONAL**).
  - Set this based on the maximum party size in your game.
  - false = Use 3 actors (33% each).
  - true = Use 4 actors (25% each).
- `EnableDebugMessages`: Whether to display debug message boxes during battle (**OPTIONAL**).
  - false = No debug messages (default).
  - true = Show detailed message boxes for damage detection, limit gain calculations, etc.
  - Useful for testing and troubleshooting, but should be disabled for release versions.
- `MaxActorId`: Maximum actor ID to check for configuration in the INI file (**OPTIONAL**).
  - Default is 20 if not specified.
  - Increase this value if your game uses actor IDs higher than 20.
  - This setting allows the plugin to support any number of actors in your database.

### Ultimate Limit Bar Display Settings

- `DrawUltimateBar`: Whether to display the ultimate limit bar on the battle screen (**OPTIONAL**).
  - true = Draw the gauge bar (default).
  - false = Do not draw the gauge bar.
- `UseVerticalBar`: Controls the orientation of the Ultimate Limit Bar gauge (**OPTIONAL**).
  - false = Horizontal bar (default).
  - true = Vertical bar, fills from bottom to top.
- `UltimateBarSwitchId`: Switch ID to control ultimate bar visibility (**OPTIONAL**).
  - 0 = Always visible (default).
  - Any value between 1-999 = Use the specified switch ID to control visibility.
  - When the switch is ON, the bar is visible; when OFF, the bar is hidden.
- `UltimateBarBgX`: X position for background/foreground images (**OPTIONAL**, default for horizontal: 17, default for vertical: 8).
- `UltimateBarBgY`: Y position for background/foreground images (**OPTIONAL**, default for horizontal: 153, default for vertical: 16).
- `UltimateBarBarX`: X position for the bar image (0% fill) (**OPTIONAL**, default for horizontal: 18, default for vertical: 8).
- `UltimateBarBarY`: Y position for the bar image (**OPTIONAL**, default for horizontal: 153, default for vertical: 17).
  - For horizontal bars: This is the actual Y position of the bar.
  - For vertical bars: This is the TOP position of the bar container, not the bottom where the fill starts.
  - Important: For vertical bars, the plugin calculates the bottom position internally based on UltimateBarBarY + UltimateBarHeight.
- `UltimateBarWidth`: Width in pixels for the bar at 100% fill in horizontal mode (**OPTIONAL**, default: 284).
- `UltimateBarHeight`: Height in pixels for the bar at 100% fill in vertical mode (**OPTIONAL**, default: 206).
  - For vertical bars, this defines the total height of the bar container.

### Sound Effect Settings

- `PlaySound100Percent`: Whether to play a sound when the ultimate bar reaches 100% (**OPTIONAL**).
  - true = Play sound effect.
  - false = No sound effect (default).
- `Sound100PercentFile`: Name of the sound effect file to play (**OPTIONAL**).
  - Use the name of a valid RPG Maker 2000/2003 sound effect without file extension.
  - Examples: "jingle1", "flash2", "cat".
  - If specified, this will automatically set `PlaySound100Percent` to true.
  - If not specified but `PlaySound100Percent` is true, "flash1" will be used as default.
- `Sound100PercentVolume`: Volume of the sound effect (0-100) (**OPTIONAL**, default: 100).
- `Sound100PercentSpeed`: Speed of the sound effect (100 = normal) (**OPTIONAL**, default: 100).
- `Sound100PercentPan`: Pan value of the sound effect (0-100, 50 = center) (**OPTIONAL**, default: 50).

### Bar Animation Settings

- `BarUseAnimation`: Whether to use frame-based animation for the bar (**OPTIONAL**).
  - true = Use animation.
  - false = No animation (default).
- `BarFrameCount`: Number of frames in the bar.png image (**OPTIONAL**, default: 1).
  - For horizontal bar layout: Frames must be stacked vertically in a single image.
  - For vertical bar layout: Frames must be stacked horizontally in a single image.
  - This different stacking method for vertical bars prevents requiring extremely tall images.
  - Example for horizontal bar: A 1x70 pixel image with 10 frames would have each frame be 1x7 pixels.
  - Example for vertical bar: A 70x1 pixel image with 10 frames would have each frame be 7x1 pixels.
- `BarAnimationSpeed`: Controls how fast the animation plays (**OPTIONAL**, default: 5).
  - Higher values = slower animation.
  - Value represents the number of game frames to wait before advancing to the next animation frame.
- `UnfilledFrames`: Comma-separated list of frame indices to use when bar is not full (0-99%) (**OPTIONAL**).
  - Frame indices are 0-based (first frame is 0, second is 1, etc.).
  - If not specified, defaults to frame 0.
  - Example: "0,1,2" will cycle through the first three frames.
- `FilledFrames`: Comma-separated list of frame indices to use when bar is full (100%) (**OPTIONAL**).
  - Frame indices are 0-based (first frame is 0, second is 1, etc.).
  - If not specified, defaults to all frames except the first one.
  - Example: "3,4,5,6,7,8,9" will cycle through frames 3-9.

### Background Animation Settings

- `BgUseAnimation`: Whether to use frame-based animation for the background (**OPTIONAL**).
  - true = Use animation.
  - false = No animation (default).
- `BgFrameCount`: Number of frames in the background.png image (**OPTIONAL**, default: 1).
  - For horizontal bar layout: Frames must be stacked vertically in a single image.
  - For vertical bar layout: Frames must be stacked horizontally in a single image.
- `BgAnimationSpeed`: Controls how fast the animation plays (**OPTIONAL**, default: 5).
  - Higher values = slower animation.
  - Value represents the number of game frames to wait before advancing to the next animation frame.
- `BgUnfilledFrames`: Comma-separated list of frame indices to use when bar is not full (0-99%) (**OPTIONAL**).
  - Frame indices are 0-based (first frame is 0, second is 1, etc.).
  - If not specified, defaults to frame 0.
  - Example: "0" will use only the first frame when bar is not full.
- `BgFilledFrames`: Comma-separated list of frame indices to use when bar is full (100%) (**OPTIONAL**).
  - Frame indices are 0-based (first frame is 0, second is 1, etc.).
  - If not specified, defaults to all frames except the first one.
  - Example: "1,2,3" will cycle through frames 1-3 when bar is full.

### Foreground Animation Settings

- `FgUseAnimation`: Whether to use frame-based animation for the foreground (**OPTIONAL**).
  - true = Use animation.
  - false = No animation (default).
- `FgFrameCount`: Number of frames in the foreground.png image (**OPTIONAL**, default: 1).
  - For horizontal bar layout: Frames must be stacked vertically in a single image.
  - For vertical bar layout: Frames must be stacked horizontally in a single image.
- `FgAnimationSpeed`: Controls how fast the animation plays (**OPTIONAL**, default: 5).
  - Higher values = slower animation.
  - Value represents the number of game frames to wait before advancing to the next animation frame.
- `FgUnfilledFrames`: Comma-separated list of frame indices to use when bar is not full (0-99%) (**OPTIONAL**).
  - Frame indices are 0-based (first frame is 0, second is 1, etc.).
  - If not specified, defaults to frame 0.
  - Example: "0" will use only the first frame when bar is not full.
- `FgFilledFrames`: Comma-separated list of frame indices to use when bar is full (100%) (**OPTIONAL**).
  - Frame indices are 0-based (first frame is 0, second is 1, etc.).
  - If not specified, defaults to all frames except the first one.
  - Example: "1,2" will cycle through frames 1-2 when bar is full.

### Actor Configuration

For each actor you want to include in the limit break system, you need to specify all five of the following settings:

```ini
; Actor 1 configuration
Actor1LimitVarID=10         ; Variable ID storing the limit break value (0-100)
Actor1ModeVarID=21          ; Variable ID that determines the current mode
Actor1DefaultMode=0         ; Fallback mode if ModeVarID is outside valid range
Actor1LimitSkillVarID=31    ; Variable ID storing the limit break skill ID to use
Actor1DefaultLimitSkillID=125 ; Fallback skill ID if LimitSkillVarID is <= 0
Actor1UltimateLimitSkillID=150 ; The skill ID to use when Actor 1 uses the Ultimate Limit command

; Actor 2 configuration
Actor2LimitVarID=11
Actor2ModeVarID=22
Actor2DefaultMode=1
Actor2LimitSkillVarID=32
Actor2DefaultLimitSkillID=51
Actor2UltimateLimitSkillID=151
```

The plugin supports configuration for actors with IDs from 1 up to the value specified in `MaxActorId` (default: 20). You only need to configure the actors you want to use with the limit break system.

**Important Note About Actor IDs**: The plugin uses the actor's database ID (not their battle position) to match configurations. For example, if Actor 1 in your database is in the second position in battle, the plugin will still use the Actor1* settings for that character. Make sure to configure each actor according to their database ID, not their battle position.

**Limit Skill ID Determination**: When an actor uses their limit break, the skill ID is determined as follows:
- If the variable specified by `ActorXLimitSkillVarID` contains a positive value (> 0), that skill ID is used.
- Otherwise, the `ActorXDefaultLimitSkillID` is used.
- This allows you to dynamically change an actor's limit break skill during gameplay.

All five settings are required for each actor you want to configure. If any setting is missing for an actor, the plugin will show an error message at startup.

**Note about unconfigured actors:** Actors that are not configured in the DynRPG.ini file will be silently ignored by the plugin. For example, if you only configure Actor1 and Actor2, but Actor3 is in your party during battle, Actor3 will not gain limit or participate in the ultimate limit bar calculations. This allows you to selectively enable the limit break system for specific actors in your game. It's perfectly valid to have no actors configured at all, in which case the limit break system will be effectively disabled.

### Limit Gain Modes



- 0 = Stoic: Gain when actor takes damage.

- 1 = Warrior: Gain when actor deals damage.

- 2 = Comrade: Gain when allies are damaged.

- 3 = Healer: Gain when actor heals others.

- 4 = Knight: Gain when actor deals OR takes damage.



The mode for each actor is determined by the variable specified in `ActorXModeVarID`:

- If the variable value is negative (< 0): The actor will not gain any limit.

- If the variable value is 0-4: The actor will use that specific mode.

- If the variable value is 5 or higher: The actor will use their `DefaultMode` instead.



This allows you to dynamically change an actor's limit gain mode during gameplay by updating the corresponding variable.



#### Limit Gain Formulas



Each mode uses a different formula to calculate limit gain:



- **Stoic (0)**: `(damageTaken * 30) / actorMaxHP * multiplier`

  - Gain is proportional to damage taken as a percentage of the actor's max HP.

  - Only direct damage to the actor counts.

  

- **Warrior (1)**: For each monster hit: `min(16, (damageDealt * 30) / monsterMaxHP) * multiplier`

  - Gain is capped at 16% per monster, proportional to damage dealt as a percentage of the monster's max HP.

  - Total gain is the sum from all monsters hit.

  - Each monster's contribution is calculated separately before summing.

  

- **Comrade (2)**: `(damageToAlliesExceptSelf * 20) / actorMaxHP * multiplier`

  - Gain is proportional to damage taken by other party members as a percentage of the actor's max HP.

  - Only counts damage to other actors, not self-damage.

  - All ally damage is summed before calculating the percentage.

  

- **Healer (3)**: `(totalHealingDone * 16) / totalTargetMaxHP * multiplier`

  - Gain is proportional to healing done as a percentage of the healed actors' total max HP.

  - Only counts positive healing (HP increase).

  - All healing is summed before calculating the percentage.

  - The total max HP is the sum of all healed targets' max HP.

  

- **Knight (4)**: Combines both Warrior and Stoic formulas.

  - When dealing damage: Same as Warrior mode.

  - When taking damage: Same as Stoic mode.

  - Both gains can occur in the same action.

  - Each part is calculated independently with its own multiplier.



**Important Notes About Limit Gain:**

- All calculations happen after the action is complete.

- Multi-hit attacks/skills are properly tracked (damage is summed per target).

- Equipment multipliers are applied after the base percentage is calculated.

- Limit values are capped at 100% maximum.

- Negative multipliers are allowed but total multiplier is capped at 0 minimum.

- Damage/healing must be greater than 0 to trigger limit gain.

- Actors with negative mode values will not gain any limit.



### Equipment Multipliers



Equipment multipliers allow specific equipment items to modify an actor's limit gain.

- Format: `EquipXXX=multiplier` where XXX is the equipment ID.

- The multiplier is added to the base multiplier of 1.0.

- Example: `Equip54=1.0` means equipment ID 54 adds +1.0 to the base multiplier (total 2.0x).

- Negative values are allowed (but total multiplier is capped at 0 minimum).



## Required Image Files



The ultimate limit bar can use up to three image files placed in the `DynRessource\LimitBreak` folder:



1. `background.png` - Background image for the gauge (optional).

2. `bar.png` - The actual bar that fills up (required).

3. `foreground.png` - Foreground image drawn on top of the bar (optional).



**Important Image Requirements:**

- All images must use RPG Maker's default mask color (magenta #FF00FF) for transparency

- Image dimensions must be evenly divisible by the frame count when using animation

- For horizontal bars with animation: Total height must be (frame height × frame count)

- For vertical bars with animation: Total width must be (frame width × frame count)

- Images with invalid dimensions will fail to load or display incorrectly



**Critical bar.png Requirements:**

- For horizontal bar layout:

  - Image must be exactly 1 pixel wide

  - Height per frame can be any size you want

  - First pixel of each frame must be magenta (#FF00FF)

  - Example: For 10 frames with 8px frame height, image would be 1×80 pixels

  - The magenta pixels should be at positions (0,0), (0,8), (0,16), etc.



- For vertical bar layout:

  - Image must be exactly 1 pixel high

  - Width per frame can be any size you want

  - First pixel of each frame must be magenta (#FF00FF)

  - Example: For 10 frames with 8px frame width, image would be 80×1 pixels

  - The magenta pixels should be at positions (0,0), (8,0), (16,0), etc.



The magenta pixels are used as reference points for the fill calculation, so they must be present and correctly positioned. The rest of each frame can use any colors you want for your bar design.



If using animation with the horizontal bar layout, each image file should contain all animation frames stacked vertically. For example, if you have 10 frames and each frame is 1 pixel high, the image should be 1x10 pixels.



If using animation with the vertical bar layout, each image file should contain all animation frames stacked horizontally. For example, if you have 10 frames and each frame is 10 pixels wide, the image should be 100x1 pixels. This horizontal stacking for vertical bars allows for more practical image sizes when using many animation frames.



While background.png and foreground.png are optional, bar.png is strictly required and must be present in the DynRessource\LimitBreak folder. The game will not execute if bar.png is missing or invalid.



## In-Game Usage



1. Create a battle command with the ID specified in `LimitCommandId`.



   **Important Command Setup Requirements:**

   - The command must be a basic "Attack" type command, not a skill menu or subtype

   - The command name can be anything (e.g., "Limit", "Special", "Break")

   - When creating the Ultimate Limit command, use the same settings

   - Other command types (skill menus, item, etc.) should use different IDs



   ![Battle Commands Example](https://i.imgur.com/7cHZdUN.gif)



2. Set up variables for each actor's limit gauge, mode, and skill ID.

3. Create a common event in the database that swaps an actor's Fight command with the Limit command.



   **IMPORTANT**: The Fight command must be in the first position (top) of the actor's battle command list, 

   and when swapping to Limit or Ultimate commands, they must also be placed in this first position. 

   The plugin specifically checks the first command slot when determining which skill to use.

   This is required because:

   - The plugin only checks the first command slot for Limit/Ultimate commands

   - The plugin only converts Attack/Double Attack commands into limit skills

   - Commands in other positions or of other types will not trigger limit skills



   ![Common Event Example](https://i.imgur.com/EKPtppk.gif)



4. For every Monster Group in your game, set up four event pages:

   - **Page 1:** Condition → Turns Elapsed 0 + 0 (pre-battle only)

   - **Page 2:** Condition → Turns Elapsed 1 + 1 (every turn after the first)

   - **Page 3:** Condition → Ultimate Variable > -1 (tracks normal limit command changes)

   - **Page 4:** Condition → Ultimate Variable > 99 (tracks ultimate limit command changes)

   

   In all four pages, call the common event you created above. This ensures proper command switching:

   - Pages 1 and 2 handle regular turn-based command updates

   - Page 3 ensures commands update when limit values change

   - Page 4 ensures commands update when ultimate limit becomes available



   ![Monster Group Event Pages Example](https://i.imgur.com/R21B7uQ.gif)



5. Equip items with multiplier effects to boost limit gain.

6. When an actor's limit gauge reaches 100%, they can use their limit break skill.

7. If using the switch-controlled visibility, turn the specified switch ON/OFF to show/hide the ultimate bar.

8. If using sound effects, the plugin will automatically play the configured sound when the ultimate bar reaches 100%.



## Ultimate Limit Bar



The Ultimate Limit Bar is filled based on the limit values of actors in battle, with special rules:



1. The entire Ultimate Limit system can be disabled by setting `UltimateLimitVarId=0` in DynRPG.ini.



2. When enabled, the bar is only calculated when the party is at full capacity:

   - In 3-actor mode: Only when exactly 3 actors are in the party.

   - In 4-actor mode: Only when exactly 4 actors are in the party.

   - If the party is not at full capacity, the ultimate bar is set to 0.



3. Each actor position contributes equally to the ultimate bar:

   - With 3 actors: Each position contributes 33% to the ultimate bar.

   - With 4 actors: Each position contributes 25% to the ultimate bar.



4. Unconfigured actors count as having 0% limit, reducing the ultimate bar's maximum potential:

   - If all actors are configured and at 100%, the ultimate bar will be at 100%.

   - If some actors are not configured, their positions still count but contribute 0%.



For example, in 3-actor mode:

- If all 3 actors are configured and at 100% limit: Ultimate bar = 100%.

- If 2 actors are configured (both at 100%) and 1 is not: Ultimate bar = 67%.

- If 1 actor is configured (at 100%) and 2 are not: Ultimate bar = 33%.

- If no actors are configured: Ultimate bar = 0%.



## Ultimate Limit Command



The plugin can optionally support a separate "Ultimate Limit" command that uses a different skill when the Ultimate Limit bar is at 100%:



- `UltimateLimitCommandId`: The ID of your "Ultimate Limit" battle command (**OPTIONAL**).

  - When set, this enables a secondary limit break system that uses the ultimate limit bar.

  - Set to 0 to disable this feature.

  - The command can be called anything you want in-game (e.g., "Ultimate," "Overdrive," "Team Attack").

  - Must be set to a valid command ID in your database.



- `ActorXUltimateLimitSkillID`: The skill ID to use when Actor X uses the Ultimate Limit command.

  - Each actor can have their own unique ultimate limit skill.

  - These skills will only be used when:

    1. The actor uses the Ultimate Limit command (defined by UltimateLimitCommandId).

    2. The ultimate limit bar is at 100%.

  - If an actor doesn't have an ultimate limit skill configured, they won't be able to use the command effectively.



When an actor selects the Ultimate Limit command and the ultimate limit bar is at 100%, their attack will be replaced with their specific ultimate limit skill. After using an ultimate limit skill, the ultimate limit bar will reset to 0%.



## Important Implementation Notes

**Limit Bar Display:** This plugin only renders the Ultimate Limit Bar - individual actor limit bars are not displayed by the plugin itself. It's up to the end user to implement visual representations of individual limit gauges. This can be accomplished using pictures when the [PicsInBattle](https://cherrytree.at/cms/download/2010/07/12/dl-14-picsinbattle-patch.html?did=14) patch is applied, or through creative use of face sets. For an excellent example of limit bar implementation using the gauge battle layout, see how [Theia - The Crimson Eclipse](http://www.theiarpg.com/) handles this feature!

**Battle Layout Compatibility:** While this plugin is designed to work optimally with the gauge battle layout, it is fully compatible with traditional and alternative battle layouts as well. The ultimate limit bar will display correctly regardless of your chosen battle system layout. However, please note that the bar may appear slightly before the window is drawn, which could momentarily affect visual consistency depending on your setup.



## Troubleshooting



If you encounter issues:



- Check that the `limit_break.dll` file is located in the DynPlugins folder.

- Verify your `DynRPG.ini` configuration for syntax errors.

- Enable `EnableDebugMessages=true` to see detailed information about limit gain during battles.



## Credits



Special thanks to:



- Cherry (David Trapp) for [DynRPG](https://rpg-maker.cherrytree.at/dynrpg/).

- rewrking aka PepsiOtaku for [DynRPG](https://github.com/rewrking/DynRPG).

- CortiWins for his [plugin source codes](https://github.com/CortiWins/CortiPluginCollection) which helped with this implementation. 