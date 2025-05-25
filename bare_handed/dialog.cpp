/*
 * Dialog handling for the BareHanded plugin.
 * Simple utility to display message boxes to the user.
 */

namespace Dialog
{
    /**
     * @brief Display a message box to the user
     * 
     * @param message Message to display
     * @param title Title of the message box
     */
    void Show(const std::string& message, const std::string& title = "BareHanded") {
        MessageBoxA(nullptr, message.c_str(), title.c_str(), MB_OK | MB_ICONINFORMATION);
    }
}
