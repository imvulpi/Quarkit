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