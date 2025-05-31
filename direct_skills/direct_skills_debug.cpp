/**
 * @file direct_skills_debug.cpp
 * @brief Debugging utilities for the DirectSkills plugin.
 * @details Provides console output functionality for debugging purposes.
 */

/**
 * @namespace Debug
 * @brief Contains debugging utilities and console management functions.
 */
namespace Debug
{
    /** @brief Global flag to control console output. */
    bool enableConsole = false;

    /** @brief Tracks whether the console has been initialized. */
    bool consoleInitialized = false;

    /**
     * @brief Initializes a console window for debug output.
     * @details Creates and attaches a console window to the application
     *          if one doesn't already exist. It redirects standard output and input
     *          to this console, allowing for debug messages to be displayed.
     * @note This function is idempotent - it will only initialize the console once.
     *       Subsequent calls will have no effect until cleanupConsole() is called.
     * @warning On Windows, this uses the Win32 API's AllocConsole() function,
     *          which may not be available on all platforms.
     * @see cleanupConsole()
     */
    void initConsole() {
        if (!consoleInitialized) {
            if (AllocConsole()) {
                // Redirect standard streams to console
                freopen("CONOUT$", "w", stdout);
                freopen("CONIN$", "r", stdin);

                // Mark console as initialized
                consoleInitialized = true;

                // Reset stream error states
                std::cout.clear();
                std::cin.clear();
            }
        }
    }

    /**
     * @brief Cleans up and releases console resources.
     * @details Properly closes the console window and releases all
     *          associated system resources. It should be called when debug output
     *          is no longer needed to prevent resource leaks.
     * @note This function is safe to call even if the console isn't initialized.
     *       It will simply return without taking any action in that case.
     * @see initConsole()
     */
    void cleanupConsole() {
        if (consoleInitialized) {
            // Ensure all output is written
            std::cout.flush();

            // Close redirected streams
            fclose(stdout);
            fclose(stdin);

            // Release console resources
            FreeConsole();

            // Reset initialization state
            consoleInitialized = false;
        }
    }
} // namespace Debug
