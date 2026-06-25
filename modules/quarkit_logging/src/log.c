#ifdef QUARKIT_LOGGING_MODULE
#include "log.h"
#include "windows_glue.h"
#include "quarkit_glue.h"

__declspec(dllimport) HANDLE __stdcall GetStdHandle(DWORD nStdHandle);
__declspec(dllimport) WIN_BOOL __stdcall WriteFile(HANDLE hFile, unsigned char* lpVuffer, DWORD nNumberOfBytesToWrite, LPDWORD lpNumberOfBytesWritten, void* lpOverlapped);
__declspec(dllimport) int __stdcall lstrlenA(LPCSTR lpString);

/// @brief Logs a simple message to STD output.
/// @param msg Contents of the message.
void print(const char* msg) {
    HANDLE hOut = GetStdHandle(((DWORD)-11));
    if(hOut != INVALID_HANDLE_VALUE){
        WriteFile(hOut, (unsigned char*)msg, (DWORD)lstrlenA(msg), NULL, NULL);
    }
}
#endif