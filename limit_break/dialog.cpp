/* 
 * Dialog functions for the Limit Break plugin.
 * Contains helper functions for showing debug messages.
 */

namespace Dialog
{

/**
 * @brief Displays an integer value in a message box
 * 
 * @param value The integer value to display
 * @param caption The window caption/title for the message box
 * 
 * @note This function is primarily used for debugging purposes
 */
void Show(int value, const std::string& caption)
{
    std::stringstream stream;
    stream.clear();
    stream << value;
    MessageBoxA(NULL, stream.str().c_str(), caption.c_str(), MB_OK);
}

/**
 * @brief Displays a text message in a message box
 * 
 * @param text The text message to display
 * @param caption The window caption/title for the message box
 * 
 * @note This function is primarily used for debugging purposes
 */
void Show(const std::string& text, const std::string& caption)
{
    MessageBoxA(NULL, text.c_str(), caption.c_str(), MB_OK);
}

/**
 * @brief Displays a text message with the default "Limit Break Debug" caption
 * 
 * @param value The text message to display
 * 
 * @note Convenience function that uses the default caption
 */
void Show(const std::string& value)
{
    Show(value, "Limit Break Debug");
}

/**
 * @brief Displays a message only once during program execution
 * 
 * @param text The text message to display
 * @param caption The window caption/title for the message box
 * 
 * @note Useful for notifications that should only appear once
 *       regardless of how many times the code path is executed
 */
void ShowOnce(const std::string& text, const std::string& caption)
{
    static std::map<std::string, bool> shownMessages;
    
    // Create a key combining caption and text to ensure uniqueness
    std::string key = caption + ":" + text;
    
    // Only show if this exact message hasn't been shown before
    if (shownMessages.find(key) == shownMessages.end()) {
        Show(text, caption);
        shownMessages[key] = true;
    }
}

} 