#include "windows_glue.h"
#include "quarkit_glue.h"

#define NULL 0
#define FILE_APPEND_DATA (0x0004)
#define CREATE_ALWAYS 2
#define FILE_ATTRIBUTE_NORMAL 0x00000080
#define GENERIC_WRITE 0x40000000
#define HEAP_ZERO_MEMORY 0x00000008

__declspec(dllimport) extern void* __stdcall GetProcessHeap(void);
__declspec(dllimport) extern void* __stdcall HeapAlloc(void* hHeap, unsigned long dwFlags, size_t dwBytes);
__declspec(dllimport) extern void* __stdcall HeapReAlloc(void* hHeap, unsigned long dwFlags, void* lpMem, size_t dwBytes);
__declspec(dllimport) extern int   __stdcall HeapFree(void* hHeap, unsigned long dwFlags, void* lpMem);

void* malloc(size_t size) {
    void* heap = GetProcessHeap();
    if (!heap) return (void*)0;
    return HeapAlloc(heap, 0, size);
}

void* calloc(size_t num, size_t size) {
    void* heap = GetProcessHeap();
    if (!heap) return (void*)0;
    return HeapAlloc(heap, HEAP_ZERO_MEMORY, num * size);
}

void* realloc(void* ptr, size_t size) {
    void* heap = GetProcessHeap();
    if (!heap) return (void*)0;
    if (!ptr) return HeapAlloc(heap, 0, size);
    return HeapReAlloc(heap, 0, ptr, size);
}

void free(void* ptr) {
    if (!ptr) return;
    void* heap = GetProcessHeap();
    if (heap) {
        HeapFree(heap, 0, ptr);
    }
}

int memcmp(const void* buf1, const void* buf2, size_t count) {
    const unsigned char* s1 = (const unsigned char*)buf1;
    const unsigned char* s2 = (const unsigned char*)buf2;
    
    for (size_t i = 0; i < count; i++) {
        if (s1[i] != s2[i]) {
            return (s1[i] < s2[i]) ? -1 : 1;
        }
    }
    return 0;
}

void* memcpy(void* destination, const void* source, size_t count) {
    unsigned char* dest = (unsigned char*)destination;
    const unsigned char* src = (const unsigned char*)source;
    for (size_t i = 0; i < count; i++) {
        dest[i] = src[i];
    }
    return destination;
}

void* memset(void* dest, int value, size_t count) {
    unsigned char* ptr = (unsigned char*)dest;
    for (size_t i = 0; i < count; i++) {
        ptr[i] = (unsigned char)value;
    }
    return dest;
}

void* memmove(void* destination, const void* source, size_t count) {
    unsigned char* dest = (unsigned char*)destination;
    const unsigned char* src = (const unsigned char*)source;

    // Check for memory overlap. If source is behind destination, 
    // copy backwards to prevent overwriting uncopied data.
    if (src < dest && dest < src + count) {
        for (size_t i = count; i > 0; i--) {
            dest[i - 1] = src[i - 1];
        }
    } else {
        // No overlap or safe overlap, copy forwards
        for (size_t i = 0; i < count; i++) {
            dest[i] = src[i];
        }
    }
    return destination;
}

#ifdef GLUE_SHARED
int __stdcall DllMainCRTStartup(void* hinstDLL, uint32_t fdwReason, void* lpvReserved) {
    return 1; 
}
#endif