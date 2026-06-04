#include "log.h"
#include "quarkit_glue.h"
#include "windows_glue.h"
#include "quarkit-extraction.h"

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
__declspec(dllimport) void* __stdcall HeapAlloc(HANDLE hHeap, DWORD dwFlags, size_t dwBytes);
__declspec(dllimport) WIN_BOOL __stdcall HeapFree(HANDLE hHeap, DWORD dwFlags, LPVOID lpMem);
__declspec(dllimport) WIN_BOOL __stdcall CloseHandle(HANDLE handle);

#ifndef QUARKIT_PAYLOAD_NAME
#define QUARKIT_PAYLOAD_NAME "payload.bin"
#endif

#ifndef QUARKIT_EXE_LAUNCH
#define QUARKIT_EXE_LAUNCH "app.exe"
#endif

#ifndef QUARKIT_PAYLOAD_SIZE
#define QUARKIT_PAYLOAD_SIZE 0
#endif

/// @brief Extracts and decompresses the data to the specified file.
void quarkit_QkBrieflzExtractor_init(){
    const unsigned char* payload_start = g_quarkit_payload.data;
    const unsigned char* payload_end = g_quarkit_payload.data + g_quarkit_payload.size;

    unsigned long blocksize = DEFAULT_BLOCK_SIZE;
	size_t max_block_size = blz_max_block_size(blocksize);

    HANDLE handle = GetProcessHeap();
    unsigned char* decompressed = (unsigned char *) HeapAlloc(handle, 0, QUARKIT_PAYLOAD_SIZE * 10); // Very unlikely ratio, BUT TODO: make shorthand to read the actual size before compression
    unsigned char* current_ptr = payload_start;
    unsigned char* write_ptr = decompressed;

    size_t depacked_size = 0;
    size_t total_decompressed_size = 0;
    while(current_ptr < payload_end - HEADER_SIZE){
        size_t hdr_packedsize, hdr_depackedsize;

        hdr_packedsize = (size_t) read_be32(current_ptr + 2 * 4);
        hdr_depackedsize = (size_t) read_be32(current_ptr + 4 * 4);

        if (read_be32(current_ptr + 0 * 4) != 0x626C7A1AUL /* "blz\x1A" */
            || read_be32(current_ptr + 1 * 4) != 1) {
            print("invalid header in compressed file");
            goto err_out;
        }

        /* Check blocksize is sufficient */
		if (hdr_packedsize > max_block_size
		 || hdr_depackedsize > blocksize) {
			print("compressed file requires a bigger block size");
            goto err_out;
		}

        current_ptr += HEADER_SIZE;
        depacked_size = blz_depack(current_ptr, write_ptr, (unsigned long) hdr_depackedsize);
        current_ptr += hdr_packedsize; 
        write_ptr += depacked_size;
        total_decompressed_size += depacked_size;
        
        if (depacked_size != hdr_depackedsize) {
            if (current_ptr >= payload_end) {
                break;
            } else {
                print("Error: Decompression mismatch inside the stream.");
                goto err_out;
            }
        }
    }

    g_quarkit_payload.data = decompressed;
    g_quarkit_payload.is_allocated = 1;
    g_quarkit_payload.size = total_decompressed_size;
    return;    
err_out:
    if (decompressed) {
        HeapFree(handle, 0, decompressed);
    }
    return;
}
