#pragma once

#define NULL 0
#define FILE_APPEND_DATA (0x0004)
#define CREATE_ALWAYS 2
#define FILE_ATTRIBUTE_NORMAL 0x00000080
#define GENERIC_WRITE 0x40000000
#if __SIZEOF_POINTER__ == 8
    #define INVALID_HANDLE_VALUE ((HANDLE)(long long)-1)
#else
    #define INVALID_HANDLE_VALUE ((HANDLE)(long)-1)
#endif

typedef __SIZE_TYPE__ size_t;
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