#include "windows_glue.h"

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