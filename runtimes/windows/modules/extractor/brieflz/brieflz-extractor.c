#ifdef MODULE_EXTRACTION
#include "../extractor.h"
#include "../../log.h"
#include "../../../../shared/glue.h"

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

/// @brief Pointer to the start of the binary payload.
extern unsigned char _payload_start[];

/// @brief Pointer to the end of the binary payload.
extern unsigned char _payload_end[];

__declspec(dllimport) HANDLE GetProcessHeap();
__declspec(dllimport) HANDLE CreateFileA(LPCSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, void* _lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile);
__declspec(dllimport) void* HeapAlloc(HANDLE hHeap, DWORD dwFlags, size_t dwBytes);
__declspec(dllimport) WIN_BOOL HeapFree(HANDLE hHeap, DWORD dwFlags, LPVOID lpMem);
__declspec(dllimport) WIN_BOOL CloseHandle(HANDLE handle);
__declspec(dllimport) WIN_BOOL WriteFile(HANDLE handle, LPVOID lpBuffer, DWORD nNumberOfBytesToWrite, LPDWORD lpNumberOfBytesWritten, void* _lpOverlapped);

/// @brief Extracts and decompresses the data to the specified file.
/// @param filename Path to the file.
void extract(const char* filename){
    size_t src_len = (_payload_end - _payload_start);
    unsigned long blocksize = DEFAULT_BLOCK_SIZE;
	size_t max_block_size = blz_max_block_size(blocksize);

    HANDLE handle = GetProcessHeap();
    HANDLE hFile = CreateFileA(filename, FILE_APPEND_DATA, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    unsigned char* data = (unsigned char *) HeapAlloc(handle, 0, blocksize);
    unsigned char* packed = (unsigned char *) HeapAlloc(handle, 0, max_block_size);

    unsigned char* currentPtr = _payload_start;
    size_t depackedsize = 0;
    while(currentPtr < _payload_end - HEADER_SIZE){
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
    CloseHandle(hFile);
    HeapFree(handle, 0, data);
    HeapFree(handle, 0, packed);
    
    return;
}

#endif