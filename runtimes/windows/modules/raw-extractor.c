#ifdef MODULE_EXTRACTION
#include <windows.h>

/// @brief Pointer to the start of the binary payload.
extern unsigned char _payload_start[];

/// @brief Pointer to the end of the binary payload.
extern unsigned char _payload_end[];

/// @brief Extracts raw binary data to the specified file.
/// @remarks This just places the data in whatever format it is, but does not decompress it or extract into individual files.
/// @param filename Path to the file.
void extract(const char* filename){
    HANDLE hFile = CreateFileA(filename, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if(hFile != INVALID_HANDLE_VALUE){
        DWORD written;
        WriteFile(hFile, (const char*)_payload_start, (DWORD)(_payload_end - _payload_start), &written, NULL);
        CloseHandle(hFile);
    }
}

#endif