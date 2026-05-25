#ifdef MODULE_EXTRACTION
#include "../extractor.h"
#include "../../log.h"
#include "../../../../shared/glue.h"
#include "../../../../../modules/xz/liblzma/common/stream_decoder.h"

// Define the PAYLOAD_SIZE option to not guess the size.

/// @brief Pointer to the start of the binary payload.
extern unsigned char _payload_start[];

/// @brief Pointer to the end of the binary payload.
extern unsigned char _payload_end[];

__declspec(dllimport) HANDLE CreateFileA(LPCSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, void* _lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile);
__declspec(dllimport) WIN_BOOL CloseHandle(HANDLE handle);
__declspec(dllimport) WIN_BOOL WriteFile(HANDLE handle, LPVOID lpBuffer, DWORD nNumberOfBytesToWrite, LPDWORD lpNumberOfBytesWritten, void* _lpOverlapped);
__declspec(dllimport) HANDLE GetProcessHeap();
__declspec(dllimport) void* HeapAlloc(HANDLE hHeap, DWORD dwFlags, size_t dwBytes);
__declspec(dllimport) WIN_BOOL HeapFree(HANDLE hHeap, DWORD dwFlags, LPVOID lpMem);
__declspec(dllimport) void* __stdcall HeapReAlloc(HANDLE hHeap, unsigned long dwFlags, void* lpMem, size_t dwBytes);

/**
 * Decompresses an XZ/LZMA2 file from memory to a newly allocated memory buffer.
 * 
 * @param start_ptr   Pointer to the beginning of the compressed data.
 * @param end_ptr     Pointer to the byte immediately following the compressed data.
 * @param out_size    Pointer to a size_t variable where the uncompressed size will be stored.
 * @return            Pointer to the allocated uncompressed data buffer, or NULL on failure.
 */
uint8_t* decompress_xz_memory(const uint8_t* start_ptr, const uint8_t* end_ptr, size_t* out_size) {
    if (!start_ptr || !end_ptr || start_ptr >= end_ptr || !out_size) {
        return NULL;
    }

    HANDLE handle = GetProcessHeap();
    size_t compressed_size = (size_t)(end_ptr - start_ptr); // Compressed size of the payload included in the .exe
    
    #ifdef PAYLOAD_SIZE
        uint8_t* uncompressed_buffer = (unsigned char *) HeapAlloc(handle, 0, PAYLOAD_SIZE);
    #else
        // Guess the size based on the compressed size:
        size_t estimated_uncompressed_size = compressed_size * 4; 
        uint8_t* uncompressed_buffer = (unsigned char *) HeapAlloc(handle, 0, estimated_uncompressed_size);
        if (!uncompressed_buffer) {
            return NULL;
        }
    #endif

    unsigned long long memlimit = UINT64_MAX; // No strict memory limit
    size_t in_pos = 0;
    size_t out_pos = 0;

    lzma_ret ret = lzma_stream_buffer_decode(
        &memlimit,
        0,    // Flags 0 (default)
        NULL, // Custom allocator (NULL uses our malloc/free glue internally)
        start_ptr, &in_pos, // Input buffer and progress index
        compressed_size,
        uncompressed_buffer, &out_pos, // Output buffer and progress index
        estimated_uncompressed_size
    );

    if (ret == LZMA_BUF_ERROR) {
        // This error means our output buffer was too small. 
        HeapFree(handle, 0, uncompressed_buffer);
        return NULL;
    }

    if (ret != LZMA_OK) {
        // Something went wrong (data corruption, invalid XZ header, etc.)
        HeapFree(handle, 0, uncompressed_buffer);
        return NULL;
    }

    // Shrink the buffer to fit the exact uncompressed payload
    uint8_t* exact_buffer = (uint8_t*)HeapReAlloc(handle, 0, uncompressed_buffer, out_pos);
    if (!exact_buffer) {
        // If realloc fails to shrink, the original buffer is still valid
        *out_size = out_pos;
        return uncompressed_buffer;
    }

    *out_size = out_pos;
    return exact_buffer;
}

/// @brief Extracts raw binary data to the specified file.
/// @remarks This just places the data in whatever format it is, but does not decompress it or extract into individual files.
/// @param filename Path to the file.
void extract(const char* filename){
    size_t decompressed_size = 0;
    uint8_t* decompressed = decompress_xz_memory(_payload_start, _payload_end, &decompressed_size);
    if(decompressed == NULL){
        print("XZfail\n"); // Minimize strings (Maybe later a code?)
    }else{
        HANDLE hFile = CreateFileA(filename, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if(hFile != INVALID_HANDLE_VALUE){
            DWORD written;
            WriteFile(hFile, decompressed, (DWORD)(decompressed_size), &written, NULL);
            CloseHandle(hFile);
        }
        HeapFree(GetProcessHeap(), 0, decompressed);
    }
}

#endif