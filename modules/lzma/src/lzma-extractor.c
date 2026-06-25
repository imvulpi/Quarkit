#include "log.h"
#include "quarkit_glue.h"
#include "windows_glue.h"
#include "quarkit-extraction.h"
#include "../deps/liblzma/common/stream_decoder.h"

__declspec(dllimport) HANDLE __stdcall CreateFileA(LPCSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, void* _lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile);
__declspec(dllimport) WIN_BOOL __stdcall CloseHandle(HANDLE handle);
__declspec(dllimport) WIN_BOOL __stdcall WriteFile(HANDLE handle, LPVOID lpBuffer, DWORD nNumberOfBytesToWrite, LPDWORD lpNumberOfBytesWritten, void* _lpOverlapped);

// These are used but imported by the glue when using liblzma:  (We get the glue from stream_decoder.h)
__declspec(dllimport) HANDLE __stdcall GetProcessHeap();
__declspec(dllimport) void* __stdcall HeapAlloc(HANDLE hHeap, DWORD dwFlags, size_t dwBytes);
__declspec(dllimport) WIN_BOOL __stdcall HeapFree(HANDLE hHeap, DWORD dwFlags, LPVOID lpMem);
__declspec(dllimport) void* __stdcall HeapReAlloc(HANDLE hHeap, unsigned long dwFlags, void* lpMem, size_t dwBytes);

extern LZMA_API(lzma_ret) lzma_stream_buffer_decode(uint64_t *memlimit, uint32_t flags,
		const lzma_allocator *allocator,
		const uint8_t *in, size_t *in_pos, size_t in_size,
		uint8_t *out, size_t *out_pos, size_t out_size);

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
    
    #ifdef QUARKIT_OG_PAYLOAD_SIZE
        size_t uncompressed_size = QUARKIT_OG_PAYLOAD_SIZE; 
        uint8_t* uncompressed_buffer = (unsigned char *) HeapAlloc(handle, 0, uncompressed_size);
        if (!uncompressed_buffer) {
            return NULL;
        }
    #else
        // Guess the size based on the compressed size:
        size_t uncompressed_size = compressed_size * 4; // *4 ratio, but could technically be too small. 
        uint8_t* uncompressed_buffer = (unsigned char *) HeapAlloc(handle, 0, uncompressed_size);
        if (!uncompressed_buffer) {
            return NULL;
        }
    #endif

    unsigned long long memlimit = UINT64_MAX;
    size_t in_pos = 0;
    size_t out_pos = 0;

    lzma_ret ret = lzma_stream_buffer_decode(
        &memlimit,
        0,    // Flags 0 (default)
        NULL, // Custom allocator (NULL uses our malloc/free glue internally)
        start_ptr, &in_pos, // Input buffer and progress index
        compressed_size,
        uncompressed_buffer, &out_pos, // Output buffer and progress index
        uncompressed_size
    );

    if (ret == LZMA_BUF_ERROR) { // This error means our output buffer was too small.
        print("LZMAE0 SBUF\n");
        HeapFree(handle, 0, uncompressed_buffer);
        return NULL;
    }

    if (ret != LZMA_OK) { // Something else went wrong (data corruption, invalid XZ header, etc.)
        print("LZMAE1 OTHER\n");
        HeapFree(handle, 0, uncompressed_buffer);
        return NULL;
    }

    // Shrink the buffer to fit the exact uncompressed payload
    uint8_t* exact_buffer = (uint8_t*)HeapReAlloc(handle, 0, uncompressed_buffer, out_pos);
    if (!exact_buffer) { // If realloc fails to shrink, the original buffer is still valid
        print("LZMAW0 REALL FAIL\n");
        *out_size = out_pos;
        return uncompressed_buffer;
    }

    *out_size = out_pos;
    return exact_buffer;
}

/// @brief Extracts raw binary data to the specified file.
/// @remarks This just places the data in whatever format it is, but does not decompress it or extract into individual files.
/// @param filename Path to the file.
void quarkit_lzma_init(const char* filename){
    size_t decompressed_size = 0;
    uint8_t* decompressed = decompress_xz_memory(g_quarkit_payload.data, g_quarkit_payload.data + g_quarkit_payload.size, &decompressed_size);
    if(decompressed == NULL){
        print("LZMA-FAIL\n");
    }else{
        print("LZMA-SUCCESS\n");
        g_quarkit_payload.data = decompressed;
        g_quarkit_payload.is_allocated = 1;
        g_quarkit_payload.size = decompressed_size;
    }
}

