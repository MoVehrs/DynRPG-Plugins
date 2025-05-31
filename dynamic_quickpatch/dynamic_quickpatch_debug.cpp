/**
 * @file dynamic_quickpatch_debug.cpp
 * @brief Debugging utilities for the DynamicQuickPatch plugin.
 * @details Provides console output functionality for debugging purposes.
 */

/**
 * @namespace Debug
 * @brief Contains debugging utilities and console management functions.
 * @details Manages console window creation, cleanup, and output redirection
 *          for debug message display.
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
        if (!consoleInitialized && enableConsole) {
            if (AllocConsole()) {
                // Redirect standard output and input streams
                freopen("CONOUT$", "w", stdout);
                freopen("CONIN$", "r", stdin);

                // Mark console as ready for use
                consoleInitialized = true;

                // Clear any stream error states
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
            // Ensure pending output is written
            std::cout.flush();

            // Close redirected stream handles
            fclose(stdout);
            fclose(stdin);

            // Free system console resources
            FreeConsole();

            // Mark console as uninitialized
            consoleInitialized = false;
        }
    }
} // namespace Debug
