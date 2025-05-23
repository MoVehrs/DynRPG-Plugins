/*
 * Configuration handling for the Limit Break plugin.
 * Loads and manages all settings from DynRPG.ini.
 */

namespace LimitBreakConfig
{
// ========================================================================
// Configuration variables (loaded from DynRPG.ini)
// ========================================================================

// Limit Break Command Configuration
static int limitCommandId = 0;  // ID of the "Limit" command in battle (must be configured in DynRPG.ini)
static bool limitCommandSelected = false; // Flag to track if Limit command was selected

// Ultimate Limit Skill Configuration
static int ultimateLimitVarId = 0;  // Variable ID for the Ultimate Limit Bar (must be configured in DynRPG.ini)
static bool useFourActorsForUltimate = false;  // false = 3 actors (33% each), true = 4 actors (25% each)
static int ultimateLimitCommandId = 0;  // ID of the "Ultimate Limit" command in battle
static bool ultimateLimitCommandSelected = false; // Flag to track if Ultimate Limit command was selected

// Debug Configuration
static bool enableDebugMessages = false;  // Whether to show debug message boxes

// Actor Configuration
static int maxActorId = 20;  // Maximum actor ID to check for configuration (default: 20)

// Actor configuration
static std::map<int, std::tuple<int, int, int, int, int>> actorConfig;

// Equipment-Based Multipliers for Limit Gain
// Key: Equipment Item ID, Value: Multiplier value to add to base 1.0
static std::map<short, float> equipmentMultipliers;

// Configuration for enabling and positioning the ultimate limit bar
// Determines whether the ultimate limit bar should be drawn at all
static bool drawUltimateBar = true;
// X coordinate for the background image of the ultimate bar (from left edge of screen)
static int ultimateBarBgX = 160;
// Y coordinate for the background image of the ultimate bar (from top edge of screen)
static int ultimateBarBgY = 16;
// X coordinate for the bar indicator itself (typically inside the background image)
static int ultimateBarBarX = 164;
// Y coordinate for the bar indicator itself (typically inside the background image)
static int ultimateBarBarY = 20;
// Width of the ultimate bar in pixels (used for horizontal bar calculation)
static int ultimateBarWidth = 120;
// Height of the ultimate bar in pixels (used for vertical bar calculation)
static int ultimateBarHeight = 120;  // New: Height for vertical bar
// When true, bar fills from bottom to top; when false, bar fills from left to right
static bool useVerticalBar = false;  // New: Toggle between horizontal and vertical bar
// Switch ID to control bar visibility (0 = disabled, valid range: 1-999)
static int ultimateBarSwitchId = 0;  // Switch ID to control bar visibility (0 = disabled)

// Sound effect configuration for when ultimate bar reaches 100%
// When true, a sound effect will play once when the bar fills completely
static bool playSound100Percent = false;
// Filename of the sound effect to play (without extension)
static std::string sound100PercentFile = "flash1";  // Default sound if not specified
// Volume level for the sound effect (0-100)
static int sound100PercentVolume = 100;
// Playback speed for the sound effect (0-100)
static int sound100PercentSpeed = 100;
// Left-right pan for the sound effect (0=left, 50=center, 100=right)
static int sound100PercentPan = 50;
// Internal flag to track if the sound has already been played at 100%
static bool wasAt100Percent = false;  // Track if bar was already at 100% to avoid repeating sound

// Animation configuration for the progress bar element
// Total number of animation frames contained in the bar image
static int barFrameCount = 1;
// Height of each animation frame (calculated automatically from image)
static int barFrameHeight = 0;
// Width of each animation frame for vertical bars (calculated automatically)
static int barFrameWidth = 0;
// Animation speed control - higher values create slower animations
static int barAnimationSpeed = 5;
// Tracks which animation frame is currently being displayed
static int currentBarFrame = 0;
// Internal counter for controlling animation timing
static int barAnimationCounter = 0;
// Master switch to enable/disable bar animation
static bool barUseAnimation = false;
// Sequence of frame indices to display when bar is not at 100%
static std::vector<int> unfilledFrames;
// Sequence of frame indices to display when bar is at 100%
static std::vector<int> filledFrames;

// Animation configuration for the background image behind the progress bar
// Total number of animation frames contained in the background image
static int bgFrameCount = 1;
// Height of each background animation frame (calculated automatically)
static int bgFrameHeight = 0;
// Width of each background frame for vertical bars (calculated automatically)
static int bgFrameWidth = 0;
// Background animation speed - higher values create slower animations
static int bgAnimationSpeed = 5;
// Tracks which background frame is currently being displayed
static int currentBgFrame = 0;
// Internal counter for controlling background animation timing
static int bgAnimationCounter = 0;
// Master switch to enable/disable background animation
static bool bgUseAnimation = false;
// Sequence of background frames to display when bar is not at 100%
static std::vector<int> bgUnfilledFrames;
// Sequence of background frames to display when bar is at 100%
static std::vector<int> bgFilledFrames;

// Animation configuration for the foreground overlay on top of the progress bar
// Total number of animation frames contained in the foreground image
static int fgFrameCount = 1;
// Height of each foreground animation frame (calculated automatically)
static int fgFrameHeight = 0;
// Width of each foreground frame for vertical bars (calculated automatically)
static int fgFrameWidth = 0;
// Foreground animation speed - higher values create slower animations
static int fgAnimationSpeed = 5;
// Tracks which foreground frame is currently being displayed
static int currentFgFrame = 0;
// Internal counter for controlling foreground animation timing
static int fgAnimationCounter = 0;
// Master switch to enable/disable foreground animation
static bool fgUseAnimation = false;
// Sequence of foreground frames to display when bar is not at 100%
static std::vector<int> fgUnfilledFrames;
// Sequence of foreground frames to display when bar is at 100%
static std::vector<int> fgFilledFrames;

/**
 * @brief Converts a string to an integer with error handling
 * 
 * @param str The string to convert
 * @param defaultValue The value to return if conversion fails
 * @return int The parsed integer or defaultValue if parsing fails
 * 
 * @note Safely handles invalid input by returning the default value
 */
int stringToInt(const std::string& str, int defaultValue = 0) {
    try {
        return std::stoi(str);
    } catch (...) {
        return defaultValue;
    }
}

/**
 * @brief Converts a string to a float with error handling
 * 
 * @param str The string to convert
 * @param defaultValue The value to return if conversion fails
 * @return float The parsed float or defaultValue if parsing fails
 * 
 * @note Safely handles invalid input by returning the default value
 */
float stringToFloat(const std::string& str, float defaultValue = 0.0f) {
    try {
        return std::stof(str);
    } catch (...) {
        return defaultValue;
    }
}

/**
 * @brief Converts a string to a boolean with support for various formats
 * 
 * @param str The string to convert
 * @param defaultValue The value to return if conversion fails
 * @return bool The parsed boolean or defaultValue if string doesn't match known formats
 * 
 * @note Supports multiple boolean representations: true/false, 1/0, yes/no, y/n, on/off
 */
bool stringToBool(const std::string& str, bool defaultValue = false) {
    if (str == "true" || str == "1" || str == "yes" || str == "y" || str == "on") {
        return true;
    } else if (str == "false" || str == "0" || str == "no" || str == "n" || str == "off") {
        return false;
    }
    return defaultValue;
}

/**
 * @brief Parses a comma-separated list of integers
 * 
 * @param str The comma-separated string to parse
 * @return std::vector<int> Vector containing all successfully parsed integers
 * 
 * @note Skips any invalid entries, only returning successfully parsed values
 */
std::vector<int> parseIntList(const std::string& str) {
    std::vector<int> result;
    std::string current;
    
    for (char c : str) {
        if (c == ',') {
            if (!current.empty()) {
                try {
                    result.push_back(std::stoi(current));
                } catch (...) {
                    // Skip invalid entries
                }
                current.clear();
            }
        } else if (isdigit(c)) {
            current += c;
        }
    }
    
    // Don't forget the last number if there's no trailing comma
    if (!current.empty()) {
        try {
            result.push_back(std::stoi(current));
        } catch (...) {
            // Skip invalid entries
        }
    }
    
    return result;
}

/**
 * @brief Loads and processes all configuration from DynRPG.ini
 * 
 * @param pluginName The name of the plugin section in DynRPG.ini
 * @return bool True if critical configuration is valid, false if required settings are missing
 * 
 * @note This function initializes all plugin settings from the configuration file
 *       and displays error messages for invalid or missing settings
 */
bool LoadConfig(char *pluginName) {
    // Load configuration dictionary from DynRPG.ini using the plugin name as section
    std::map<std::string, std::string> config = RPG::loadConfiguration(pluginName);
    
    // Process core game settings
    // These control basic plugin functionality and must be properly configured
    
    // Load basic settings
    if (config.find("LimitCommandId") != config.end()) {
        limitCommandId = stringToInt(config["LimitCommandId"], 0);
    }
    
    if (config.find("UltimateLimitVarId") != config.end()) {
        ultimateLimitVarId = stringToInt(config["UltimateLimitVarId"], 0);
    }
    
    if (config.find("UseFourActorsForUltimate") != config.end()) {
        useFourActorsForUltimate = stringToBool(config["UseFourActorsForUltimate"], false);
    }
    
    if (config.find("EnableDebugMessages") != config.end()) {
        enableDebugMessages = stringToBool(config["EnableDebugMessages"], false);
    }
    
    if (config.find("MaxActorId") != config.end()) {
        maxActorId = stringToInt(config["MaxActorId"], 20);
        // Ensure maxActorId is at least 1
        maxActorId = std::max(1, maxActorId);
    }
	
    // Process UI configuration for the Ultimate Bar display
    if (config.find("DrawUltimateBar") != config.end()) {
        drawUltimateBar = stringToBool(config["DrawUltimateBar"], true);
    }
    
    // Load vertical bar configuration first to determine defaults
    if (config.find("UseVerticalBar") != config.end()) useVerticalBar = stringToBool(config["UseVerticalBar"], false);
    
    // Set position and size values based on vertical/horizontal bar configuration
    if (useVerticalBar) {
        // Vertical bar defaults
        if (config.find("UltimateBarBgX") != config.end()) ultimateBarBgX = stringToInt(config["UltimateBarBgX"], 8);
        else ultimateBarBgX = 8;
        
        if (config.find("UltimateBarBgY") != config.end()) ultimateBarBgY = stringToInt(config["UltimateBarBgY"], 16);
        else ultimateBarBgY = 16;
        
        if (config.find("UltimateBarBarX") != config.end()) ultimateBarBarX = stringToInt(config["UltimateBarBarX"], 8);
        else ultimateBarBarX = 8;
        
        if (config.find("UltimateBarBarY") != config.end()) ultimateBarBarY = stringToInt(config["UltimateBarBarY"], 17);
        else ultimateBarBarY = 17;
        
        if (config.find("UltimateBarHeight") != config.end()) ultimateBarHeight = stringToInt(config["UltimateBarHeight"], 206);
        else ultimateBarHeight = 206;
        
        // Width still uses the same default for vertical bars
        if (config.find("UltimateBarWidth") != config.end()) ultimateBarWidth = stringToInt(config["UltimateBarWidth"], 120);
    } else {
        // Horizontal bar defaults
        if (config.find("UltimateBarBgX") != config.end()) ultimateBarBgX = stringToInt(config["UltimateBarBgX"], 17);
        else ultimateBarBgX = 17;
        
        if (config.find("UltimateBarBgY") != config.end()) ultimateBarBgY = stringToInt(config["UltimateBarBgY"], 153);
        else ultimateBarBgY = 153;
        
        if (config.find("UltimateBarBarX") != config.end()) ultimateBarBarX = stringToInt(config["UltimateBarBarX"], 18);
        else ultimateBarBarX = 18;
        
        if (config.find("UltimateBarBarY") != config.end()) ultimateBarBarY = stringToInt(config["UltimateBarBarY"], 153);
        else ultimateBarBarY = 153;
        
        if (config.find("UltimateBarWidth") != config.end()) ultimateBarWidth = stringToInt(config["UltimateBarWidth"], 284);
        else ultimateBarWidth = 284;
        
        // Height still uses the same default for horizontal bars
        if (config.find("UltimateBarHeight") != config.end()) ultimateBarHeight = stringToInt(config["UltimateBarHeight"], 120);
    }
	
    // Load switch ID for bar visibility
    if (config.find("UltimateBarSwitchId") != config.end()) {
        ultimateBarSwitchId = stringToInt(config["UltimateBarSwitchId"], 0);
        
        // Store the actual switch ID for later use
        // No need to access the switch value during startup
		
    }
    
    // Process sound effect settings for when bar reaches 100%
    if (config.find("PlaySound100Percent") != config.end()) {
        playSound100Percent = stringToBool(config["PlaySound100Percent"], false);
    }
    
    if (config.find("Sound100PercentFile") != config.end()) {
        sound100PercentFile = config["Sound100PercentFile"];
        // If a sound file is specified, enable sound playback automatically
        // This allows users to only specify the sound file without needing to set PlaySound100Percent
        if (!sound100PercentFile.empty()) {
            playSound100Percent = true;
        }
    } else if (playSound100Percent) {
        // If PlaySound100Percent is true but no sound file specified, keep the default "flash1"
        // The default is already set in the variable initialization
        if (enableDebugMessages) {
            std::string msg = "No sound file specified, using default: " + sound100PercentFile;
            Dialog::Show(msg, "Ultimate Bar Sound");
        }
    }
    
    if (config.find("Sound100PercentVolume") != config.end()) {
        sound100PercentVolume = stringToInt(config["Sound100PercentVolume"], 100);
        // Clamp volume to valid range (0-100, where 0 is silent and 100 is maximum volume)
        sound100PercentVolume = std::max(0, std::min(100, sound100PercentVolume));
    }
    
    if (config.find("Sound100PercentSpeed") != config.end()) {
        sound100PercentSpeed = stringToInt(config["Sound100PercentSpeed"], 100);
        // Ensure speed is positive (100 is normal speed, higher values speed up playback)
        sound100PercentSpeed = std::max(1, sound100PercentSpeed);
    }
    
    if (config.find("Sound100PercentPan") != config.end()) {
        sound100PercentPan = stringToInt(config["Sound100PercentPan"], 50);
        // Clamp pan to valid range (0=left speaker, 50=center, 100=right speaker)
        sound100PercentPan = std::max(0, std::min(100, sound100PercentPan));
    }
    
    // Load bar animation settings
    if (config.find("BarUseAnimation") != config.end()) {
        barUseAnimation = stringToBool(config["BarUseAnimation"], false);
    }
    
    if (config.find("BarFrameCount") != config.end()) {
        barFrameCount = stringToInt(config["BarFrameCount"], 1);
        // Ensure at least 1 frame
        barFrameCount = std::max(1, barFrameCount);
    }
    
    if (config.find("BarAnimationSpeed") != config.end()) {
        barAnimationSpeed = stringToInt(config["BarAnimationSpeed"], 5);
        // Ensure at least 1 frame delay
        barAnimationSpeed = std::max(1, barAnimationSpeed);
    }
    
    // Load frame sequences for the bar animation
    // These define which animation frames are shown for different bar states
    if (config.find("UnfilledFrames") != config.end()) {
        // Parse comma-separated indices of frames to use when bar is not at 100%
        unfilledFrames = parseIntList(config["UnfilledFrames"]);
        
        // Validate frame indices to ensure they're within the available frames
        for (size_t i = 0; i < unfilledFrames.size(); i++) {
            if (unfilledFrames[i] < 0 || unfilledFrames[i] >= barFrameCount) {
                // Remove invalid frame indices that would cause out-of-bounds access
                unfilledFrames.erase(unfilledFrames.begin() + i);
                i--;
            }
        }
    }
    
    if (config.find("FilledFrames") != config.end()) {
        // Parse comma-separated indices of frames to use when bar is at 100%
        // This often uses different frames to highlight the fully filled state
        filledFrames = parseIntList(config["FilledFrames"]);
        
        // Validate frame indices to ensure they're within the available frames
        for (size_t i = 0; i < filledFrames.size(); i++) {
            if (filledFrames[i] < 0 || filledFrames[i] >= barFrameCount) {
                // Remove invalid frame indices that would cause out-of-bounds access
                filledFrames.erase(filledFrames.begin() + i);
                i--;
            }
        }
    }
    
    // If animation is enabled but no frames are defined, set default sequences
    if (barUseAnimation && barFrameCount > 1) {
        if (unfilledFrames.empty()) {
            // Default to first frame for unfilled
            unfilledFrames.push_back(0);
        }
        
        if (filledFrames.empty()) {
            // Default to all frames except the first for filled
            for (int i = 1; i < barFrameCount; i++) {
                filledFrames.push_back(i);
            }
            // If there's only one frame, use it
            if (filledFrames.empty()) {
                filledFrames.push_back(0);
            }
        }
    }
    
    // Load background animation settings
    if (config.find("BgUseAnimation") != config.end()) {
        bgUseAnimation = stringToBool(config["BgUseAnimation"], false);
    }
    
    if (config.find("BgFrameCount") != config.end()) {
        bgFrameCount = stringToInt(config["BgFrameCount"], 1);
        // Ensure at least 1 frame
        bgFrameCount = std::max(1, bgFrameCount);
    }
    
    if (config.find("BgAnimationSpeed") != config.end()) {
        bgAnimationSpeed = stringToInt(config["BgAnimationSpeed"], 5);
        // Ensure at least 1 frame delay
        bgAnimationSpeed = std::max(1, bgAnimationSpeed);
    }
    
    // Load background frame sequence
    if (config.find("BgUnfilledFrames") != config.end()) {
        bgUnfilledFrames = parseIntList(config["BgUnfilledFrames"]);
        
        // Validate frame indices
        for (size_t i = 0; i < bgUnfilledFrames.size(); i++) {
            if (bgUnfilledFrames[i] < 0 || bgUnfilledFrames[i] >= bgFrameCount) {
                // Remove invalid frame indices
                bgUnfilledFrames.erase(bgUnfilledFrames.begin() + i);
                i--;
            }
        }
    }
    
    if (config.find("BgFilledFrames") != config.end()) {
        bgFilledFrames = parseIntList(config["BgFilledFrames"]);
        
        // Validate frame indices
        for (size_t i = 0; i < bgFilledFrames.size(); i++) {
            if (bgFilledFrames[i] < 0 || bgFilledFrames[i] >= bgFrameCount) {
                // Remove invalid frame indices
                bgFilledFrames.erase(bgFilledFrames.begin() + i);
                i--;
            }
        }
    }
    
    // If background animation is enabled but no frames are defined, set default sequence
    if (bgUseAnimation && bgFrameCount > 1) {
        if (bgUnfilledFrames.empty()) {
            // Default to first frame for unfilled
            bgUnfilledFrames.push_back(0);
        }
        
        if (bgFilledFrames.empty()) {
            // Default to all frames except the first for filled
            for (int i = 1; i < bgFrameCount; i++) {
                bgFilledFrames.push_back(i);
            }
            // If there's only one frame, use it
            if (bgFilledFrames.empty()) {
                bgFilledFrames.push_back(0);
            }
        }
    }
    
    // Load foreground animation settings
    if (config.find("FgUseAnimation") != config.end()) {
        fgUseAnimation = stringToBool(config["FgUseAnimation"], false);
    }
    
    if (config.find("FgFrameCount") != config.end()) {
        fgFrameCount = stringToInt(config["FgFrameCount"], 1);
        // Ensure at least 1 frame
        fgFrameCount = std::max(1, fgFrameCount);
    }
    
    if (config.find("FgAnimationSpeed") != config.end()) {
        fgAnimationSpeed = stringToInt(config["FgAnimationSpeed"], 5);
        // Ensure at least 1 frame delay
        fgAnimationSpeed = std::max(1, fgAnimationSpeed);
    }
    
    // Load foreground frame sequence
    if (config.find("FgUnfilledFrames") != config.end()) {
        fgUnfilledFrames = parseIntList(config["FgUnfilledFrames"]);
        
        // Validate frame indices
        for (size_t i = 0; i < fgUnfilledFrames.size(); i++) {
            if (fgUnfilledFrames[i] < 0 || fgUnfilledFrames[i] >= fgFrameCount) {
                // Remove invalid frame indices
                fgUnfilledFrames.erase(fgUnfilledFrames.begin() + i);
                i--;
            }
        }
    }
    
    if (config.find("FgFilledFrames") != config.end()) {
        fgFilledFrames = parseIntList(config["FgFilledFrames"]);
        
        // Validate frame indices
        for (size_t i = 0; i < fgFilledFrames.size(); i++) {
            if (fgFilledFrames[i] < 0 || fgFilledFrames[i] >= fgFrameCount) {
                // Remove invalid frame indices
                fgFilledFrames.erase(fgFilledFrames.begin() + i);
                i--;
            }
        }
    }
    
    // If foreground animation is enabled but no frames are defined, set default sequence
    if (fgUseAnimation && fgFrameCount > 1) {
        if (fgUnfilledFrames.empty()) {
            // Default to first frame for unfilled
            fgUnfilledFrames.push_back(0);
        }
        
        if (fgFilledFrames.empty()) {
            // Default to all frames except the first for filled
            for (int i = 1; i < fgFrameCount; i++) {
                fgFilledFrames.push_back(i);
            }
            // If there's only one frame, use it
            if (fgFilledFrames.empty()) {
                fgFilledFrames.push_back(0);
            }
        }
    }
    
    // Process actor-specific configurations
    actorConfig.clear();
    
    // Iterate through possible actor IDs (1 to maxActorId)
    // RPG Maker 2000/2003 actor IDs start at 1, not 0
    for (int i = 1; i <= maxActorId; ++i) { // Support up to maxActorId actors
        // Create the prefix used for all keys related to this actor (e.g., "Actor1LimitVarID")
        std::string actorPrefix = "Actor" + std::to_string(i);
        
        // Check if any configuration for this actor exists by searching for keys with this prefix
        bool actorConfigExists = false;
        for (const auto& entry : config) {
            if (entry.first.find(actorPrefix) == 0) {
                actorConfigExists = true;
                break;
            }
        }
        
        if (!actorConfigExists) {
            continue; // Skip to next actor if no config for this one
        }
        
        // Define all required configuration keys for this actor
        // Each actor needs 5 configuration values to be fully set up
        std::string limitVarKey = actorPrefix + "LimitVarID";       // Variable to store limit value (0-100%)
        std::string modeVarKey = actorPrefix + "ModeVarID";         // Variable to store current limit mode
        std::string defaultModeKey = actorPrefix + "DefaultMode";   // Default mode if variable is out of range
        std::string limitSkillVarKey = actorPrefix + "LimitSkillVarID"; // Variable to store limit skill ID
        std::string defaultLimitSkillKey = actorPrefix + "DefaultLimitSkillID"; // Default skill if variable has no value
        
        // Ensure all required keys are present for this actor
        bool allKeysPresent = true;
        std::string missingKeys;
        
        // Check each required key and track any missing ones
        if (config.find(limitVarKey) == config.end()) {
            allKeysPresent = false;
            missingKeys += limitVarKey + " ";
        }
        
        if (config.find(modeVarKey) == config.end()) {
            allKeysPresent = false;
            missingKeys += modeVarKey + " ";
        }
        
        if (config.find(defaultModeKey) == config.end()) {
            allKeysPresent = false;
            missingKeys += defaultModeKey + " ";
        }
        
        if (config.find(limitSkillVarKey) == config.end()) {
            allKeysPresent = false;
            missingKeys += limitSkillVarKey + " ";
        }
        
        if (config.find(defaultLimitSkillKey) == config.end()) {
            allKeysPresent = false;
            missingKeys += defaultLimitSkillKey + " ";
        }
        
        // Skip actors with incomplete configuration
        if (!allKeysPresent) {
            if (enableDebugMessages) {
                std::string msg = "Missing required keys for " + actorPrefix + ": " + missingKeys;
                Dialog::Show(msg, "Configuration Error");
            }
            continue; // Skip to next actor
        }
        
        // All required keys are present, read the values and convert from strings
        int limitVarId = stringToInt(config[limitVarKey]);           // Variable storing the limit gauge value
        int modeVarId = stringToInt(config[modeVarKey]);             // Variable storing the limit mode
        int defaultMode = stringToInt(config[defaultModeKey]);       // Default mode (0=Stoic, 1=Warrior, 2=Comrade, 3=Healer, 4=Knight)
        int limitSkillVarId = stringToInt(config[limitSkillVarKey]); // Variable storing the limit skill ID
        int defaultLimitSkillId = stringToInt(config[defaultLimitSkillKey]); // Default limit skill ID to use
        
        // Store the configuration in the actor map using a tuple to pack all values together
        // This allows efficient storage and retrieval of all actor settings
        actorConfig[i] = std::make_tuple(limitVarId, modeVarId, defaultMode, limitSkillVarId, defaultLimitSkillId);
    }
    
    // Process equipment-based limit gain multipliers
    equipmentMultipliers.clear();
    for (const auto& entry : config) {
        // Look for equipment entries which start with "Equip" followed by the item ID
        // Example: "Equip25 = 0.5" adds a 0.5x multiplier to equipment item #25
        if (entry.first.substr(0, 5) == "Equip") {
            try {
                // Extract the equipment ID from the key (e.g., "Equip25" -> 25)
                short equipId = std::stoi(entry.first.substr(5));
                // Parse the multiplier value from the configuration
                float multiplier = stringToFloat(entry.second, 0.0f);
                // Store the multiplier for this equipment item
                equipmentMultipliers[equipId] = multiplier;
                
                if (enableDebugMessages) {
                    std::string msg = "Loaded equipment multiplier: Item #" + 
                                     std::to_string(equipId) + " = " + 
                                     std::to_string(multiplier) + "x";
                    Dialog::Show(msg, "Equipment Multiplier");
                }
            } catch (...) {
                // Skip invalid entries that can't be parsed properly
                if (enableDebugMessages) {
                    std::string msg = "Invalid equipment multiplier: " + entry.first;
                    Dialog::Show(msg, "Configuration Error");
                }
            }
        }
    }
    
    // Load the Ultimate Limit command ID
    if (config.find("UltimateLimitCommandId") != config.end()) {
        ultimateLimitCommandId = stringToInt(config["UltimateLimitCommandId"], 0);
    }
    
    // Validate required configuration and report any critical errors
    bool configValid = true;
    std::string missingConfig;
    
    if (limitCommandId <= 0) {
        configValid = false;
        missingConfig += "LimitCommandId must be set to a positive value.\n";
    }
    
    // Show error message if configuration is invalid
    if (!configValid) {
        std::string errorMsg = "Limit Break Plugin Configuration Error:\n\n" + missingConfig + 
                              "\nPlease check your DynRPG.ini file.";
        Dialog::Show(errorMsg, "Limit Break Plugin Error");
        return false;
    }
    
    return true;
}

/**
 * @brief Gets the current mode for an actor based on configuration
 * 
 * @param actorId The database ID of the actor to check
 * @return int The active mode (0-4) or -1 if the actor should be skipped
 * 
 * @note Modes: 0=Stoic, 1=Warrior, 2=Comrade, 3=Healer, 4=Knight
 *       A return value of -1 indicates the actor should not gain limit
 */
int getActorMode(int actorId) {
    auto cfgIt = actorConfig.find(actorId);
    if (cfgIt == actorConfig.end())
        return -1; // No config for this actor - skip processing

    // Get the mode variable ID and default mode from config
    int modeVarId = std::get<1>(cfgIt->second);
    int defaultMode = std::get<2>(cfgIt->second);

    // Get the current mode value from the RPG Maker variable
    int currentMode = RPG::variables[modeVarId];

    // Apply the logic for mode selection based on variable value
    if (currentMode < 0)
        return -1; // Skip processing for this actor (no limit gain)
    else if (currentMode >= 0 && currentMode <= 4)
        return currentMode; // Valid mode range, use the variable value
    else
        return defaultMode; // Out of range, use the default mode
}

/**
 * @brief Gets the current limit break skill ID for an actor
 * 
 * @param actorId The database ID of the actor to check
 * @return int The configured skill ID or 0 if no valid configuration exists
 * 
 * @note The skill ID is determined first by checking the configured variable.
 *       If the variable contains a valid ID (>0), that ID is used.
 *       Otherwise, the default skill ID from configuration is used.
 */
int getActorLimitSkillId(int actorId) {
    auto cfgIt = actorConfig.find(actorId);
    if (cfgIt == actorConfig.end())
        return 0; // No config for this actor - return 0 (invalid skill ID)
        
    // Get the limit skill variable ID and default skill ID from config
    int limitSkillVarId = std::get<3>(cfgIt->second);
    int defaultLimitSkillId = std::get<4>(cfgIt->second);
    
    // If the variable ID is valid, get the skill ID from the variable
    if (limitSkillVarId > 0) {
        int skillId = RPG::variables[limitSkillVarId];
        // If the variable contains a valid skill ID, use it
        if (skillId > 0)
            return skillId;
    }
    
    // Otherwise use the default skill ID
    return defaultLimitSkillId;
}

/**
 * @brief Gets the Ultimate Limit skill ID for a specific actor
 * 
 * @param actorId The database ID of the actor
 * @return int The actor's Ultimate Limit skill ID, or 0 if not configured
 */
int getActorUltimateLimitSkillId(int actorId) {
    std::string key = "Actor" + std::to_string(actorId) + "UltimateLimitSkillID";
    
    // Use the same approach as other config values - lookup in the configuration map
    std::map<std::string, std::string> config = RPG::loadConfiguration("limit_break");
    if (config.find(key) != config.end()) {
        return stringToInt(config[key], 0);
    }
    
    return 0; // Return 0 if no Ultimate Limit skill is configured for this actor
}

} // namespace LimitBreakConfig 