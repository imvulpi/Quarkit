#pragma once

// Define GLUE_SHARED for shared library.

#ifndef GLUE_H
#define GLUE_H

typedef __SIZE_TYPE__ size_t;
typedef unsigned long long uint64_t;
typedef unsigned int uint32_t;
typedef unsigned short uint16_t;
typedef unsigned char uint8_t;
typedef void* HANDLE;
typedef void* LPVOID;
typedef void* HWND;
typedef void* HMODULE;
typedef unsigned long DWORD;
typedef unsigned long* LPDWORD;
typedef int WIN_BOOL; // Windows uses int for booleans 
typedef char* LPSTR;
typedef const char* LPCSTR;
typedef _Bool bool;
#define false 0
#define true  1

void* malloc(size_t size);
void* calloc(size_t num, size_t size);
void* realloc(void* ptr, size_t size);
void free(void* ptr);
int memcmp(const void* buf1, const void* buf2, size_t count);
void* memcpy(void* destination, const void* source, size_t count);
void* memset(void* dest, int value, size_t count);
void* memmove(void* destination, const void* source, size_t count);

// Disable asserts:
#undef assert
#define assert(condition) ((void)0)

#if __SIZEOF_POINTER__ == 8
    #define INVALID_HANDLE_VALUE ((HANDLE)(long long)-1)
#else
    #define INVALID_HANDLE_VALUE ((HANDLE)(long)-1)
#endif

#if defined(__x86_64__) || defined(_M_X64) || defined(__aarch64__) || defined(_M_ARM64)
    // 64-bit Systems: Pointers are 8 bytes, matching unsigned long long
    typedef unsigned long long uintptr_t;
    typedef signed long long   intptr_t;
#else
    // 32-bit Systems: Pointers are 4 bytes, matching unsigned int
    typedef unsigned int       uintptr_t;
    typedef signed int         intptr_t;
#endif

// Windows API:
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

#endif // GLUE_H