#include "log.h"
#include "quarkit_glue.h"
#include "windows_glue.h"

/* --- BriefLZ start --- */
/* From BriefLZ - see: deps/brieflz */

#if CHAR_BIT == 8
/// @brief Gets the low-order 8 bits of a value. 
#  define octet(v) ((unsigned char) (v))
#else
/// @brief Gets the low-order 8 bits of a value. 
#  define octet(v) ((v) & 0x00FF)
#endif

/// @brief Size of the header = 24.
#define HEADER_SIZE (6 * 4)

/// @brief Default block size = 1MB.
#define DEFAULT_BLOCK_SIZE (1024 * 1024UL)

/// @brief Decompresses `depacked_size` bytes of data from `src` to `dst`
/// @param src pointer to compressed data
/// @param dst pointer to where to place decompressed data
/// @param depacked_size size of decompressed data
/// @return size of decompressed data
extern unsigned long blz_depack(const void *src, void *dst, unsigned long depacked_size);

/// @brief Get max block size of compressed data.
/// @param src_size Default block size.
size_t blz_max_block_size(size_t src_size)
{
	return src_size + src_size / 8 + 64;
}

/// @brief Read a 32-bit unsigned value in network order.
static unsigned long
read_be32(const unsigned char *p)
{
	return ((unsigned long) octet(p[0]) << 24)
	     | ((unsigned long) octet(p[1]) << 16)
	     | ((unsigned long) octet(p[2]) << 8)
	     | ((unsigned long) octet(p[3]));
}

/* --- BriefLZ end --- */

__declspec(dllimport) HANDLE __stdcall GetProcessHeap();
__declspec(dllimport) HANDLE __stdcall CreateFileA(LPCSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, void* _lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile);
__declspec(dllimport) void* __stdcall HeapAlloc(HANDLE hHeap, DWORD dwFlags, size_t dwBytes);
__declspec(dllimport) WIN_BOOL __stdcall HeapFree(HANDLE hHeap, DWORD dwFlags, LPVOID lpMem);
__declspec(dllimport) WIN_BOOL __stdcall CloseHandle(HANDLE handle);
__declspec(dllimport) WIN_BOOL __stdcall WriteFile(HANDLE handle, LPVOID lpBuffer, DWORD nNumberOfBytesToWrite, LPDWORD lpNumberOfBytesWritten, void* _lpOverlapped);

__declspec(dllimport) DWORD __stdcall GetModuleFileNameA(HANDLE hModule, char* lpFilename, DWORD nSize);
__declspec(dllimport) WIN_BOOL __stdcall GetFileSizeEx(HANDLE hFile, LARGE_INTEGER* lpFileSize);
__declspec(dllimport) HANDLE __stdcall CreateFileMappingA(
    HANDLE hFile,
    void* lpFileMappingAttributes,
    DWORD flProtect,
    DWORD dwMaximumSizeHigh,
    DWORD dwMaximumSizeLow,
    const char* lpName
);

__declspec(dllimport) void* __stdcall MapViewOfFile(
    HANDLE hFileMappingObject,
    DWORD dwDesiredAccess,
    DWORD dwFileOffsetHigh,
    DWORD dwFileOffsetLow,
    size_t dwNumberOfBytesToMap
);

__declspec(dllimport) WIN_BOOL __stdcall UnmapViewOfFile(
    const void* lpBaseAddress
);

#define MAX_PATH          260
#define GENERIC_READ      0x80000000L
#define FILE_SHARE_READ   0x00000001
#define OPEN_EXISTING     3
#define CREATE_ALWAYS     2
#define FILE_ATTRIBUTE_NORMAL 0x00000080

#define PAGE_READONLY     0x02
#define FILE_MAP_READ     0x0004

#ifndef QUARKIT_PAYLOAD_SIZE
#define QUARKIT_PAYLOAD_SIZE 0
#endif

#ifndef QUARKIT_PAYLOAD_NAME
#define QUARKIT_PAYLOAD_NAME "payload.bin"
#endif

#ifndef QUARKIT_EXE_LAUNCH
#define QUARKIT_EXE_LAUNCH "app.exe"
#endif

/// @brief Extracts and decompresses the data to the specified file.
void quarkit_QkBrieflzExtractor_init(){
    char exePath[MAX_PATH];
    if (GetModuleFileNameA(NULL, exePath, MAX_PATH) == 0) {
        return;
    }

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
    
    LARGE_INTEGER totalFileSize;
    if (!GetFileSizeEx(hExeFile, &totalFileSize)) {
        CloseHandle(hExeFile);
        return;
    }

    size_t payload_size = QUARKIT_PAYLOAD_SIZE;
    size_t shell_size = (size_t)(totalFileSize.QuadPart - payload_size);
    HANDLE hMapping = CreateFileMappingA(
        hExeFile, 
        NULL, 
        PAGE_READONLY, 
        0, 
        0, 
        NULL
    );
    
    if (!hMapping) {
        CloseHandle(hExeFile);
        return;
    }

    const unsigned char* base_file_address = (const unsigned char*)MapViewOfFile(
        hMapping, 
        FILE_MAP_READ, 
        0, 
        0, 
        0
    );

    if(base_file_address == NULL) {
        CloseHandle(hExeFile);
        return;
    }

    const unsigned char* payload_start = base_file_address + shell_size;
    const unsigned char* payload_end = payload_start + payload_size;

    size_t src_len = (payload_end - payload_start);
    unsigned long blocksize = DEFAULT_BLOCK_SIZE;
	size_t max_block_size = blz_max_block_size(blocksize);

    HANDLE handle = GetProcessHeap();
    HANDLE hFile = CreateFileA(QUARKIT_EXE_LAUNCH, FILE_APPEND_DATA, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if(handle == NULL || hFile == INVALID_HANDLE_VALUE ) {
        return;
    }

    unsigned char* data = (unsigned char *) HeapAlloc(handle, 0, blocksize);
    unsigned char* packed = (unsigned char *) HeapAlloc(handle, 0, max_block_size);

    unsigned char* currentPtr = payload_start;
    size_t depackedsize = 0;
    while(currentPtr < payload_end - HEADER_SIZE){
        size_t hdr_packedsize, hdr_depackedsize;

        hdr_packedsize = (size_t) read_be32(currentPtr + 2 * 4);
        hdr_depackedsize = (size_t) read_be32(currentPtr + 4 * 4);

        if (read_be32(currentPtr + 0 * 4) != 0x626C7A1AUL /* "blz\x1A" */
            || read_be32(currentPtr + 1 * 4) != 1) {
            print("invalid header in compressed file");
            goto out;
        }

        /* Check blocksize is sufficient */
		if (hdr_packedsize > max_block_size
		 || hdr_depackedsize > blocksize) {
			print("compressed file requires a bigger block size");
            goto out;
		}

        currentPtr += HEADER_SIZE;
        depackedsize = blz_depack(currentPtr, data, (unsigned long) hdr_depackedsize);
        currentPtr += hdr_packedsize; 
        
        if (depackedsize != hdr_depackedsize) {
            goto out;
        }

        DWORD written;
        WriteFile(hFile, (const char*)data, (DWORD)depackedsize, &written, NULL);
    }
out:
    CloseHandle(hExeFile);
    CloseHandle(hFile);
    CloseHandle(hMapping);
    UnmapViewOfFile(base_file_address);
    HeapFree(handle, 0, data);
    HeapFree(handle, 0, packed);
    
    return;
}
