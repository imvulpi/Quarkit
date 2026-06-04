#include "log.h"
#include "quarkit_glue.h"
#include "windows_glue.h"
#include "quarkit-extraction.h"

#ifndef QUARKIT_PAYLOAD_NAME
#define QUARKIT_PAYLOAD_NAME "payload.bin"
#endif

#ifndef QUARKIT_EXE_LAUNCH
#define QUARKIT_EXE_LAUNCH "app.exe"
#endif

__declspec(dllimport) HANDLE __stdcall CreateFileA(LPCSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, void* _lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile);
__declspec(dllimport) WIN_BOOL __stdcall CloseHandle(HANDLE handle);
__declspec(dllimport) WIN_BOOL __stdcall WriteFile(HANDLE handle, LPVOID lpBuffer, DWORD nNumberOfBytesToWrite, LPDWORD lpNumberOfBytesWritten, void* _lpOverlapped);

/// @brief Extracts the data to the specified file.
void quarkit_quarkit_outputter_init(){
    HANDLE hFile = CreateFileA(QUARKIT_EXE_LAUNCH, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if(hFile != INVALID_HANDLE_VALUE){
        DWORD written;
        /// TODO: Make it output at the target directory! (For development time we keep it same directory.)
        WriteFile(hFile, (const char*)g_quarkit_payload.data, (DWORD)(g_quarkit_payload.size), &written, NULL);
        CloseHandle(hFile);
    }
}
