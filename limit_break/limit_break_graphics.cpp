/*
 * Graphics handling for the Limit Break plugin.
 * Contains functions for drawing the Ultimate Limit Bar gauge.
 */

namespace LimitBreakGraphics
{
// ========================================================================
// Graphics state variables
// ========================================================================

// Image pointers for the gauge
static RPG::Image* ultimateBarBgImg = nullptr;
static RPG::Image* ultimateBarBarImg = nullptr;
static RPG::Image* ultimateBarFgImg = nullptr;

/**
 * @brief Check if a file exists at the specified path
 *
 * @param filename The path to the file to check
 * @return bool True if the file exists, false otherwise
 *
 * @note This is used to verify image files before attempting to load them
 */
bool FileExist(const std::string& filename) {
    std::ifstream ifile(filename.c_str());
    return (bool)ifile;
}

/**
 * @brief Loads the images needed for the Ultimate Limit Bar
 *
 * @note This function:
 *       1. Loads background.png, bar.png, and foreground.png from the DynRessource\limit_break folder
 *       2. Configures image properties (alpha, mask color, etc.)
 *       3. Calculates frame dimensions if animation is enabled
 *       4. Handles both horizontal and vertical bar layouts
 *       5. Logs debug messages if image loading fails
 *
 *       Only bar.png is strictly required; the others are optional.
 */
void loadUltimateBarImages() {
    // Load background image
    if (!ultimateBarBgImg) {
        ultimateBarBgImg = RPG::Image::create();
        ultimateBarBgImg->useMaskColor = true;

        std::string bgPath = "DynRessource\\limit_break\\background.png";
        if (FileExist(bgPath)) {
            ultimateBarBgImg->loadFromFile(bgPath, false);
            ultimateBarBgImg->alpha = 255;

            // Calculate frame dimensions based on layout type
            if (LimitBreakConfig::bgUseAnimation && LimitBreakConfig::bgFrameCount > 1) {
                if (LimitBreakConfig::useVerticalBar) {
                    // For vertical bars, split frames horizontally
                    LimitBreakConfig::bgFrameWidth = ultimateBarBgImg->width / LimitBreakConfig::bgFrameCount;

                    if (LimitBreakConfig::enableDebugMessages) {
                        std::string msg = "Background loaded with horizontal animation. Size: ";
                        msg += std::to_string(ultimateBarBgImg->width) + "x" + std::to_string(ultimateBarBgImg->height);
                        msg += ", Frames: " + std::to_string(LimitBreakConfig::bgFrameCount);
                        msg += ", Frame Width: " + std::to_string(LimitBreakConfig::bgFrameWidth);
                        Dialog::Show(msg, "Ultimate Bar Debug");
                    }
                } else {
                    // For horizontal bars, split frames vertically into multiple images
                    LimitBreakConfig::bgFrameHeight = ultimateBarBgImg->height / LimitBreakConfig::bgFrameCount;

                    if (LimitBreakConfig::enableDebugMessages) {
                        std::string msg = "Background loaded with vertical animation. Size: ";
                        msg += std::to_string(ultimateBarBgImg->width) + "x" + std::to_string(ultimateBarBgImg->height);
                        msg += ", Frames: " + std::to_string(LimitBreakConfig::bgFrameCount);
                        msg += ", Frame Height: " + std::to_string(LimitBreakConfig::bgFrameHeight);
                        Dialog::Show(msg, "Ultimate Bar Debug");
                    }
                }
            } else if (LimitBreakConfig::enableDebugMessages) {
                std::string msg = "Background loaded. Size: ";
                msg += std::to_string(ultimateBarBgImg->width) + "x" + std::to_string(ultimateBarBgImg->height);
                Dialog::Show(msg, "Ultimate Bar Debug");
            }
        } else if (LimitBreakConfig::enableDebugMessages) {
            std::string msg = "Background image not found: " + bgPath;
            Dialog::Show(msg, "Ultimate Bar Debug");
        }
    }

    // Load bar image
    if (!ultimateBarBarImg) {
        ultimateBarBarImg = RPG::Image::create();
        ultimateBarBarImg->useMaskColor = true;
        // Set autoResize to true to ensure the image resizes properly when loaded
        ultimateBarBarImg->autoResize = true;

        std::string barPath = "DynRessource\\limit_break\\bar.png";
        if (FileExist(barPath)) {
            // Try loading with throwErrors=true to see any loading errors
            try {
                ultimateBarBarImg->loadFromFile(barPath, true);
                ultimateBarBarImg->alpha = 255;

                // Calculate frame dimensions based on layout type
                if (LimitBreakConfig::barUseAnimation && LimitBreakConfig::barFrameCount > 1) {
                    if (LimitBreakConfig::useVerticalBar) {
                        // For vertical bars, split frames horizontally
                        LimitBreakConfig::barFrameWidth = ultimateBarBarImg->width / LimitBreakConfig::barFrameCount;

                        if (LimitBreakConfig::enableDebugMessages) {
                            std::string msg = "Bar loaded with horizontal animation. Size: ";
                            msg += std::to_string(ultimateBarBarImg->width) + "x" + std::to_string(ultimateBarBarImg->height);
                            msg += ", Frames: " + std::to_string(LimitBreakConfig::barFrameCount);
                            msg += ", Frame Width: " + std::to_string(LimitBreakConfig::barFrameWidth);
                            Dialog::Show(msg, "Ultimate Bar Debug");
                        }
                    } else {
                        // For horizontal bars, split frames vertically into multiple images
                        LimitBreakConfig::barFrameHeight = ultimateBarBarImg->height / LimitBreakConfig::barFrameCount;

                        if (LimitBreakConfig::enableDebugMessages) {
                            std::string msg = "Bar loaded with vertical animation. Size: ";
                            msg += std::to_string(ultimateBarBarImg->width) + "x" + std::to_string(ultimateBarBarImg->height);
                            msg += ", Frames: " + std::to_string(LimitBreakConfig::barFrameCount);
                            msg += ", Frame Height: " + std::to_string(LimitBreakConfig::barFrameHeight);
                            Dialog::Show(msg, "Ultimate Bar Debug");
                        }
                    }
                } else {
                    if (LimitBreakConfig::enableDebugMessages) {
                        std::string msg = "Bar loaded without animation. Size: ";
                        msg += std::to_string(ultimateBarBarImg->width) + "x" + std::to_string(ultimateBarBarImg->height);
                        Dialog::Show(msg, "Ultimate Bar Debug");
                    }
                }
            } catch (...) {
                if (LimitBreakConfig::enableDebugMessages) {
                    Dialog::Show("Error loading bar image", "Ultimate Bar Debug");
                }
            }
        } else {
            if (LimitBreakConfig::enableDebugMessages) {
                std::string msg = "Bar image not found: " + barPath;
                Dialog::Show(msg, "Ultimate Bar Debug");
            }
        }
    }

    // Load foreground image
    if (!ultimateBarFgImg) {
        ultimateBarFgImg = RPG::Image::create();
        ultimateBarFgImg->useMaskColor = true;
        // Set autoResize to true to ensure the image resizes properly when loaded
        ultimateBarFgImg->autoResize = true;

        std::string fgPath = "DynRessource\\limit_break\\foreground.png";
        if (FileExist(fgPath)) {
            // Try loading with throwErrors=true to see any loading errors
            try {
                ultimateBarFgImg->loadFromFile(fgPath, true);
                ultimateBarFgImg->alpha = 255;

                // Calculate frame dimensions based on layout type
                if (LimitBreakConfig::fgUseAnimation && LimitBreakConfig::fgFrameCount > 1) {
                    if (LimitBreakConfig::useVerticalBar) {
                        // For vertical bars, split frames horizontally
                        LimitBreakConfig::fgFrameWidth = ultimateBarFgImg->width / LimitBreakConfig::fgFrameCount;

                        if (LimitBreakConfig::enableDebugMessages) {
                            std::string msg = "Foreground loaded with horizontal animation. Size: ";
                            msg += std::to_string(ultimateBarFgImg->width) + "x" + std::to_string(ultimateBarFgImg->height);
                            msg += ", Frames: " + std::to_string(LimitBreakConfig::fgFrameCount);
                            msg += ", Frame Width: " + std::to_string(LimitBreakConfig::fgFrameWidth);
                            Dialog::Show(msg, "Ultimate Bar Debug");
                        }
                    } else {
                        // For horizontal bars, split frames vertically into multiple images
                        LimitBreakConfig::fgFrameHeight = ultimateBarFgImg->height / LimitBreakConfig::fgFrameCount;

                        if (LimitBreakConfig::enableDebugMessages) {
                            std::string msg = "Foreground loaded with vertical animation. Size: ";
                            msg += std::to_string(ultimateBarFgImg->width) + "x" + std::to_string(ultimateBarFgImg->height);
                            msg += ", Frames: " + std::to_string(LimitBreakConfig::fgFrameCount);
                            msg += ", Frame Height: " + std::to_string(LimitBreakConfig::fgFrameHeight);
                            Dialog::Show(msg, "Ultimate Bar Debug");
                        }
                    }
                } else if (LimitBreakConfig::enableDebugMessages) {
                    std::string msg = "Foreground loaded. Size: ";
                    msg += std::to_string(ultimateBarFgImg->width) + "x" + std::to_string(ultimateBarFgImg->height);
                    Dialog::Show(msg, "Ultimate Bar Debug");
                }
            } catch (...) {
                if (LimitBreakConfig::enableDebugMessages) {
                    Dialog::Show("Error loading foreground image", "Ultimate Bar Debug");
                }
            }
        } else if (LimitBreakConfig::enableDebugMessages) {
            std::string msg = "Foreground image not found: " + fgPath;
            Dialog::Show(msg, "Ultimate Bar Debug");
        }
    }

    // If bar image failed to load, log a debug message but don't create a fallback
    if (!ultimateBarBarImg || ultimateBarBarImg->width == 0 || ultimateBarBarImg->height == 0) {
        if (LimitBreakConfig::enableDebugMessages) {
            Dialog::Show("Bar image not loaded or invalid", "Ultimate Bar Debug");
        }
    }
}

/**
 * @brief Frees resources used by the Ultimate Limit Bar images
 *
 * @note Properly destroys all image objects to prevent memory leaks
 *       and resets pointers to nullptr to avoid dangling references
 */
void freeUltimateBarImages() {
    if (ultimateBarBgImg) RPG::Image::destroy(ultimateBarBgImg);
    if (ultimateBarBarImg) RPG::Image::destroy(ultimateBarBarImg);
    if (ultimateBarFgImg) RPG::Image::destroy(ultimateBarFgImg);

    ultimateBarBgImg = nullptr;
    ultimateBarBarImg = nullptr;
    ultimateBarFgImg = nullptr;
}

/**
 * @brief Draws the Ultimate Limit Bar gauge on the battle screen
 *
 * @note This function:
 *       1. Checks various conditions to determine if the bar should be drawn:
 *          - Ultimate limit system must be enabled
 *          - Must be in battle scene
 *          - Party window must exist
 *          - Visibility switch (if configured) must be ON
 *          - Party must be at full capacity
 *       2. Loads images if not already loaded
 *       3. Calculates the fill percentage based on the ultimateLimitVarId variable
 *       4. Draws the background, bar, and foreground with proper animations
 *       5. Plays a sound effect when the bar first reaches 100%
 *       6. Updates animation frames for each component
 *
 *       The bar can be drawn in either horizontal or vertical orientation,
 *       with different animation patterns based on the fill percentage.
 */
void drawUltimateLimitBar() {
    // Only draw if all conditions are met
    if (LimitBreakConfig::ultimateLimitVarId <= 0 || !LimitBreakConfig::drawUltimateBar) {
        if (LimitBreakConfig::enableDebugMessages) {
            static bool firstTime = true;
            if (firstTime) {
                std::string msg = "Not drawing: UltimateLimitVarId=" + std::to_string(LimitBreakConfig::ultimateLimitVarId);
                msg += ", drawUltimateBar=" + std::string(LimitBreakConfig::drawUltimateBar ? "true" : "false");
                Dialog::Show(msg, "Ultimate Bar Debug");
                firstTime = false;
            }
        }
        return;
    }

    // Check if the party window exists
    if (!RPG::battleData || !RPG::battleData->winParty) {
        if (LimitBreakConfig::enableDebugMessages) {
            static bool firstTime = true;
            if (firstTime) {
                std::string msg = "Not drawing: Party window does not exist";
                Dialog::Show(msg, "Ultimate Bar Debug");
                firstTime = false;
            }
        }
        return;
    }

    // Check the visibility switch if configured
    if (LimitBreakConfig::ultimateBarSwitchId > 0) {
        // Check if the switch is ON
        // RPG::switches uses the exact switch ID (not 0-indexed)
        bool switchValue = false;

        // Only access the switch if it's in a valid range
        if (LimitBreakConfig::ultimateBarSwitchId >= 1 && LimitBreakConfig::ultimateBarSwitchId <= 999) {
            // Access with the exact switch ID
            try {
                switchValue = RPG::switches[LimitBreakConfig::ultimateBarSwitchId];
            } catch (...) {
                // If accessing the switch fails, assume it's OFF
                if (LimitBreakConfig::enableDebugMessages) {
                    static bool firstTime = true;
                    if (firstTime) {
                        std::string msg = "Error accessing switch " + std::to_string(LimitBreakConfig::ultimateBarSwitchId);
                        Dialog::Show(msg, "Ultimate Bar Debug");
                        firstTime = false;
                    }
                }
            }
        }

        if (LimitBreakConfig::enableDebugMessages) {
            static bool firstTime = true;
            if (firstTime) {
                std::string msg = "Switch " + std::to_string(LimitBreakConfig::ultimateBarSwitchId) + " value: ";
                msg += switchValue ? "ON" : "OFF";
                Dialog::Show(msg, "Ultimate Bar Debug");
                firstTime = false;
            }
        }

        if (!switchValue) {
            // Switch is OFF or couldn't be accessed, don't draw the bar
            return;
        }
    }

    int requiredPartySize = LimitBreakConfig::useFourActorsForUltimate ? 4 : 3;
    int currentPartySize = 0;
    for (int i = 0; i < 4; ++i) {
        if (RPG::Actor::partyMember(i)) currentPartySize++;
    }

    if (currentPartySize < requiredPartySize) {
        if (LimitBreakConfig::enableDebugMessages) {
            static bool firstTime = true;
            if (firstTime) {
                std::string msg = "Not drawing: Party size " + std::to_string(currentPartySize);
                msg += " < required " + std::to_string(requiredPartySize);
                Dialog::Show(msg, "Ultimate Bar Debug");
                firstTime = false;
            }
        }
        return;
    }

    loadUltimateBarImages();

    // Check if bar image is loaded properly - this is the only required image
    if (!ultimateBarBarImg || ultimateBarBarImg->width == 0 || ultimateBarBarImg->height == 0) {
        if (LimitBreakConfig::enableDebugMessages) {
            static bool firstTime = true;
            if (firstTime) {
                Dialog::Show("Bar image not loaded properly", "Ultimate Bar Debug");
                firstTime = false;
            }
        }
        return;
    }

    // Set alpha for all loaded images to ensure visibility
    ultimateBarBarImg->alpha = 255;
    if (ultimateBarBgImg) ultimateBarBgImg->alpha = 255;
    if (ultimateBarFgImg) ultimateBarFgImg->alpha = 255;

    // Get the fill percentage for the bar (0-100)
    int fill = 0;
    if (LimitBreakConfig::ultimateLimitVarId > 0) {
        int value = RPG::variables[LimitBreakConfig::ultimateLimitVarId];
        fill = std::max(0, std::min(100, value));
    }

    // Draw background if it exists
    if (ultimateBarBgImg && ultimateBarBgImg->width > 0 && ultimateBarBgImg->height > 0) {
        // Handle background animation
        if (LimitBreakConfig::bgUseAnimation && LimitBreakConfig::bgFrameCount > 1) {
            // Update animation counter
            LimitBreakConfig::bgAnimationCounter++;
            if (LimitBreakConfig::bgAnimationCounter >= LimitBreakConfig::bgAnimationSpeed) {
                LimitBreakConfig::bgAnimationCounter = 0;

                // Select which set of frames to use based on fill percentage
                std::vector<int>& activeBgFrames = (fill == 100) ? LimitBreakConfig::bgFilledFrames : LimitBreakConfig::bgUnfilledFrames;

                // If we have frames defined, cycle through them
                if (!activeBgFrames.empty()) {
                    // Find the index of the current frame in the frames list
                    int currentIndex = 0;
                    for (size_t i = 0; i < activeBgFrames.size(); i++) {
                        if (activeBgFrames[i] == LimitBreakConfig::currentBgFrame) {
                            currentIndex = i;
                            break;
                        }
                    }

                    // Move to the next frame in the sequence
                    currentIndex = (currentIndex + 1) % activeBgFrames.size();
                    LimitBreakConfig::currentBgFrame = activeBgFrames[currentIndex];
                }
                else {
                    // No frames defined, just cycle through all frames
                    LimitBreakConfig::currentBgFrame = (LimitBreakConfig::currentBgFrame + 1) % LimitBreakConfig::bgFrameCount;
                }
            }

            if (LimitBreakConfig::useVerticalBar && LimitBreakConfig::bgFrameWidth > 0) {
                // For vertical bars, use horizontal frame splitting
                // Calculate the X position for the current frame
                int frameX = LimitBreakConfig::currentBgFrame * LimitBreakConfig::bgFrameWidth;

                // Draw the current background frame
                RPG::screen->canvas->draw(
                    LimitBreakConfig::ultimateBarBgX,
                    LimitBreakConfig::ultimateBarBgY,
                    ultimateBarBgImg,
                    frameX,
                    0,
                    LimitBreakConfig::bgFrameWidth,
                    ultimateBarBgImg->height
                );
            } else if (LimitBreakConfig::bgFrameHeight > 0) {
                // For horizontal bars, use vertical frame splitting
                // Calculate the Y position for the current frame
                int frameY = LimitBreakConfig::currentBgFrame * LimitBreakConfig::bgFrameHeight;

                // Draw the current background frame
                RPG::screen->canvas->draw(
                    LimitBreakConfig::ultimateBarBgX,
                    LimitBreakConfig::ultimateBarBgY,
                    ultimateBarBgImg,
                    0,
                    frameY,
                    ultimateBarBgImg->width,
                    LimitBreakConfig::bgFrameHeight
                );
            }
        }
        else {
            // Non-animated version - just draw the background normally
            RPG::screen->canvas->draw(LimitBreakConfig::ultimateBarBgX, LimitBreakConfig::ultimateBarBgY, ultimateBarBgImg);
        }
    }

    // Check if we need to play the sound for reaching 100%
    if (LimitBreakConfig::playSound100Percent && fill == 100 && !LimitBreakConfig::wasAt100Percent && !LimitBreakConfig::sound100PercentFile.empty()) {
        // Create and play the sound
        RPG::Sound sound(LimitBreakConfig::sound100PercentFile, LimitBreakConfig::sound100PercentVolume,
                         LimitBreakConfig::sound100PercentSpeed, LimitBreakConfig::sound100PercentPan);
        sound.play();

        if (LimitBreakConfig::enableDebugMessages) {
            std::string msg = "Playing 100% sound: " + LimitBreakConfig::sound100PercentFile;
            msg += " (Vol:" + std::to_string(LimitBreakConfig::sound100PercentVolume);
            msg += ", Spd:" + std::to_string(LimitBreakConfig::sound100PercentSpeed);
            msg += ", Pan:" + std::to_string(LimitBreakConfig::sound100PercentPan) + ")";
            Dialog::Show(msg, "Ultimate Bar Sound");
        }
    }

    // Update the tracking flag for sound playback
    LimitBreakConfig::wasAt100Percent = (fill == 100);

    // Calculate the bar dimensions based on fill percentage and orientation
    int barDrawWidth = LimitBreakConfig::useVerticalBar ? 1 : (LimitBreakConfig::ultimateBarWidth * fill) / 100;
    int barDrawHeight = LimitBreakConfig::useVerticalBar ? (LimitBreakConfig::ultimateBarHeight * fill) / 100 : ultimateBarBarImg->height;

    // Handle animation frame selection
    if (LimitBreakConfig::barUseAnimation && LimitBreakConfig::barFrameCount > 1) {
        // Update animation counter
        LimitBreakConfig::barAnimationCounter++;
        if (LimitBreakConfig::barAnimationCounter >= LimitBreakConfig::barAnimationSpeed) {
            LimitBreakConfig::barAnimationCounter = 0;

            // Select which set of frames to use based on fill percentage
            std::vector<int>& activeFrames = (fill == 100) ? LimitBreakConfig::filledFrames : LimitBreakConfig::unfilledFrames;

            // If we have frames defined, cycle through them
            if (!activeFrames.empty()) {
                // Find the index of the current frame in the active frames list
                int currentIndex = 0;
                for (size_t i = 0; i < activeFrames.size(); i++) {
                    if (activeFrames[i] == LimitBreakConfig::currentBarFrame) {
                        currentIndex = i;
                        break;
                    }
                }

                // Move to the next frame in the sequence
                currentIndex = (currentIndex + 1) % activeFrames.size();
                LimitBreakConfig::currentBarFrame = activeFrames[currentIndex];
            }
            else {
                // No frames defined, just cycle through all frames
                LimitBreakConfig::currentBarFrame = (LimitBreakConfig::currentBarFrame + 1) % LimitBreakConfig::barFrameCount;
            }
        }

        if (LimitBreakConfig::useVerticalBar && LimitBreakConfig::barFrameWidth > 0) {
            // For vertical bars with horizontal frame splitting
            // Calculate the X position for the current frame
            int frameX = LimitBreakConfig::currentBarFrame * LimitBreakConfig::barFrameWidth;

            // Draw the bar with the current animation frame (vertical)
            if (barDrawHeight > 0) {
                // Stretch the bar image vertically using the current frame from horizontal strips
                for (int y = 0; y < barDrawHeight; ++y) {
                    RPG::screen->canvas->draw(
                        LimitBreakConfig::ultimateBarBarX,
                        LimitBreakConfig::ultimateBarBarY + (LimitBreakConfig::ultimateBarHeight - barDrawHeight) + y,
                        ultimateBarBarImg,
                        frameX,
                        0,
                        LimitBreakConfig::barFrameWidth,
                        1
                    );
                }
            }
        } else if (LimitBreakConfig::barFrameHeight > 0) {
            // For horizontal bars with vertical frame splitting
            // Calculate the Y position for the current frame
            int frameY = LimitBreakConfig::currentBarFrame * LimitBreakConfig::barFrameHeight;

            // Draw the bar with the current animation frame (horizontal)
            if (barDrawWidth > 0) {
                // Stretch the bar image horizontally using the current frame
                for (int x = 0; x < barDrawWidth; ++x) {
                    RPG::screen->canvas->draw(
                        LimitBreakConfig::ultimateBarBarX + x,
                        LimitBreakConfig::ultimateBarBarY,
                        ultimateBarBarImg,
                        0,
                        frameY,
                        1,
                        LimitBreakConfig::barFrameHeight
                    );
                }
            }
        }
    }
    else {
        // Non-animated version
        if (LimitBreakConfig::useVerticalBar) {
            // Draw vertical bar
            if (barDrawHeight > 0) {
                // Stretch the bar image vertically
                for (int y = 0; y < barDrawHeight; ++y) {
                    RPG::screen->canvas->draw(
                        LimitBreakConfig::ultimateBarBarX,
                        LimitBreakConfig::ultimateBarBarY + (LimitBreakConfig::ultimateBarHeight - barDrawHeight) + y,
                        ultimateBarBarImg,
                        0,
                        0,
                        ultimateBarBarImg->width,
                        1
                    );
                }
            }
        } else {
            // Draw horizontal bar
            if (barDrawWidth > 0) {
                // Stretch the bar image horizontally
                for (int x = 0; x < barDrawWidth; ++x) {
                    RPG::screen->canvas->draw(
                        LimitBreakConfig::ultimateBarBarX + x,
                        LimitBreakConfig::ultimateBarBarY,
                        ultimateBarBarImg,
                        0,
                        0,
                        1,
                        ultimateBarBarImg->height
                    );
                }
            }
        }
    }

    // Draw foreground if it exists
    if (ultimateBarFgImg && ultimateBarFgImg->width > 0 && ultimateBarFgImg->height > 0) {
        // Handle foreground animation
        if (LimitBreakConfig::fgUseAnimation && LimitBreakConfig::fgFrameCount > 1) {
            // Update animation counter
            LimitBreakConfig::fgAnimationCounter++;
            if (LimitBreakConfig::fgAnimationCounter >= LimitBreakConfig::fgAnimationSpeed) {
                LimitBreakConfig::fgAnimationCounter = 0;

                // Select which set of frames to use based on fill percentage
                std::vector<int>& activeFgFrames = (fill == 100) ? LimitBreakConfig::fgFilledFrames : LimitBreakConfig::fgUnfilledFrames;

                // If we have frames defined, cycle through them
                if (!activeFgFrames.empty()) {
                    // Find the index of the current frame in the frames list
                    int currentIndex = 0;
                    for (size_t i = 0; i < activeFgFrames.size(); i++) {
                        if (activeFgFrames[i] == LimitBreakConfig::currentFgFrame) {
                            currentIndex = i;
                            break;
                        }
                    }

                    // Move to the next frame in the sequence
                    currentIndex = (currentIndex + 1) % activeFgFrames.size();
                    LimitBreakConfig::currentFgFrame = activeFgFrames[currentIndex];
                }
                else {
                    // No frames defined, just cycle through all frames
                    LimitBreakConfig::currentFgFrame = (LimitBreakConfig::currentFgFrame + 1) % LimitBreakConfig::fgFrameCount;
                }
            }

            if (LimitBreakConfig::useVerticalBar && LimitBreakConfig::fgFrameWidth > 0) {
                // For vertical bars, use horizontal frame splitting
                // Calculate the X position for the current frame
                int frameX = LimitBreakConfig::currentFgFrame * LimitBreakConfig::fgFrameWidth;

                // Draw the current foreground frame
                RPG::screen->canvas->draw(
                    LimitBreakConfig::ultimateBarBgX,
                    LimitBreakConfig::ultimateBarBgY,
                    ultimateBarFgImg,
                    frameX,
                    0,
                    LimitBreakConfig::fgFrameWidth,
                    ultimateBarFgImg->height
                );
            } else if (LimitBreakConfig::fgFrameHeight > 0) {
                // For horizontal bars, use vertical frame splitting
                // Calculate the Y position for the current frame
                int frameY = LimitBreakConfig::currentFgFrame * LimitBreakConfig::fgFrameHeight;

                // Draw the current foreground frame
                RPG::screen->canvas->draw(
                    LimitBreakConfig::ultimateBarBgX,
                    LimitBreakConfig::ultimateBarBgY,
                    ultimateBarFgImg,
                    0,
                    frameY,
                    ultimateBarFgImg->width,
                    LimitBreakConfig::fgFrameHeight
                );
            }
        }
        else {
            // Non-animated version - just draw the foreground normally
            RPG::screen->canvas->draw(LimitBreakConfig::ultimateBarBgX, LimitBreakConfig::ultimateBarBgY, ultimateBarFgImg);
        }
    }

    if (LimitBreakConfig::enableDebugMessages) {
        static bool firstTime = true;
        if (firstTime) {
            std::string msg = "Drawing bar: fill=" + std::to_string(fill) + "%";
            if (LimitBreakConfig::useVerticalBar) {
                msg += ", height=" + std::to_string(barDrawHeight) + " (vertical)";
                if (LimitBreakConfig::barUseAnimation) {
                    msg += ", using horizontal frame splitting";
                }
            } else {
                msg += ", width=" + std::to_string(barDrawWidth) + " (horizontal)";
                if (LimitBreakConfig::barUseAnimation) {
                    msg += ", using vertical frame splitting";
                }
            }

            if (LimitBreakConfig::barUseAnimation) {
                msg += ", bar frame=" + std::to_string(LimitBreakConfig::currentBarFrame);
            }
            if (LimitBreakConfig::bgUseAnimation && ultimateBarBgImg) {
                msg += ", bg frame=" + std::to_string(LimitBreakConfig::currentBgFrame);
            }
            if (LimitBreakConfig::fgUseAnimation && ultimateBarFgImg) {
                msg += ", fg frame=" + std::to_string(LimitBreakConfig::currentFgFrame);
            }
            msg += ", at (" + std::to_string(LimitBreakConfig::ultimateBarBgX) + "," + std::to_string(LimitBreakConfig::ultimateBarBgY) + ")";
            Dialog::Show(msg, "Ultimate Bar Debug");
            firstTime = false;
        }
    }
}

} // namespace LimitBreakGraphics
