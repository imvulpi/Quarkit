// Extraction framework source

#include "quarkit_glue.h"
#include "quarkit-extraction.h"
#include "windows_glue.h"

QuarkitPayloadContext g_quarkit_payload = { .data = NULL, .size = 0, .is_allocated = 0 };

__declspec(dllimport) HANDLE __stdcall GetProcessHeap();
__declspec(dllimport) HANDLE __stdcall CreateFileA(LPCSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, void* _lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile);
__declspec(dllimport) void* __stdcall HeapAlloc(HANDLE hHeap, DWORD dwFlags, size_t dwBytes);
__declspec(dllimport) WIN_BOOL __stdcall HeapFree(HANDLE hHeap, DWORD dwFlags, LPVOID lpMem);
__declspec(dllimport) WIN_BOOL __stdcall CloseHandle(HANDLE handle);
__declspec(dllimport) DWORD __stdcall GetModuleFileNameA(HANDLE hModule, char* lpFilename, DWORD nSize);
__declspec(dllimport) WIN_BOOL __stdcall SetFilePointerEx(HANDLE hFile, LARGE_INTEGER liDistanceToMove, LARGE_INTEGER* lpNewFilePointer, DWORD dwMoveMethod);
__declspec(dllimport) WIN_BOOL __stdcall ReadFile(HANDLE hFile, LPVOID lpBuffer, DWORD nNumberOfBytesToRead, DWORD* lpNumberOfBytesRead, void* lpOverlapped);

#ifndef QUARKIT_PAYLOAD_SIZE
#define QUARKIT_PAYLOAD_SIZE 0
#endif

void quarkit_quarkit_extraction_init()
{
    char exePath[MAX_PATH];
    if (QUARKIT_PAYLOAD_SIZE == 0) return;
    if (GetModuleFileNameA(NULL, exePath, MAX_PATH) == 0) return;

    HANDLE hExeFile = CreateFileA(
        exePath, 
        GENERIC_READ, 
        FILE_SHARE_READ, 
        NULL, 
        OPEN_EXISTING, 
        FILE_ATTRIBUTE_NORMAL, 
        NULL
    );

    if (hExeFile == INVALID_HANDLE_VALUE) return;
    
    LARGE_INTEGER fileOffset;
    fileOffset.QuadPart = -((long long)QUARKIT_PAYLOAD_SIZE);

    LARGE_INTEGER newPointer;
    WIN_BOOL bSuccess = SetFilePointerEx(hExeFile, fileOffset, &newPointer, FILE_END);
    if(!bSuccess) { 
        CloseHandle(hExeFile); 
        return; 
    }

    HANDLE hHeap = GetProcessHeap();
    if(!hHeap){
        CloseHandle(hExeFile);
        return;
    }

    LPVOID heapBuffer = HeapAlloc(hHeap, 0, QUARKIT_PAYLOAD_SIZE);
    if(!heapBuffer){
        CloseHandle(hExeFile);
        return;
    }

    DWORD bytesReadNow = 0;
    WIN_BOOL bReadSuccess = ReadFile(hExeFile, heapBuffer, QUARKIT_PAYLOAD_SIZE, &bytesReadNow, NULL);
    if (!bReadSuccess) {
        HeapFree(hHeap, 0, heapBuffer);
        CloseHandle(hExeFile);
        return;
    }

    g_quarkit_payload.size = bytesReadNow;
    g_quarkit_payload.data = heapBuffer;
    g_quarkit_payload.is_allocated = 1;
}

void quarkit_quarkit_extraction_deinit()
{
    if (g_quarkit_payload.is_allocated && g_quarkit_payload.data != NULL) {
        HANDLE hHeap = GetProcessHeap();
        if(!hHeap){
            return;
        }
        
        HeapFree(hHeap, 0, g_quarkit_payload.data);
        g_quarkit_payload.data = NULL;
        g_quarkit_payload.is_allocated = 0;
        g_quarkit_payload.size = 0;
    }
}