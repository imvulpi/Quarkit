#pragma once

// Define GLUE_SHARED for shared library.

typedef void* HANDLE;
typedef void* LPVOID;
typedef void* HWND;
typedef void* HMODULE;
typedef unsigned long DWORD;
typedef unsigned long* LPDWORD;
typedef int WIN_BOOL; // Windows uses int for booleans 
typedef char* LPSTR;
typedef const char* LPCSTR;

// File and other API
#define NULL 0
#define FILE_APPEND_DATA (0x0004)
#define CREATE_ALWAYS 2
#define FILE_ATTRIBUTE_NORMAL 0x00000080
#define GENERIC_WRITE 0x40000000
#define HEAP_ZERO_MEMORY 0x00000008
#define GENERIC_READ 0x80000000L
#define FILE_SHARE_READ 0x00000001
#define OPEN_EXISTING 3
#define CREATE_ALWAYS 2
#define FILE_ATTRIBUTE_NORMAL 0x00000080
#define PAGE_READONLY 0x02
#define FILE_MAP_READ 0x0004
#define MAX_PATH 260
#define FILE_BEGIN 0
#define FILE_CURRENT 1
#define FILE_END 2

// Struct for retrieving 64-bit file sizes
typedef union _LARGE_INTEGER {
    struct {
        DWORD LowPart;
        long HighPart;
    } DUMMYSTRUCTNAME;
    __INT64_TYPE__ QuadPart;
} LARGE_INTEGER;