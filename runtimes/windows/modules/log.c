#ifdef MODULE_LOGGING
#include <windows.h>
/// @brief Logs a simple message to STD output.
/// @param msg Contents of the message.
void print(const char* msg) {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    WriteFile(hOut, msg, (DWORD)lstrlenA(msg), NULL, NULL);
}
#endif