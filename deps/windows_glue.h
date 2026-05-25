#pragma once

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

#ifndef UINT64_C
#   define UINT64_C(n) n ## ULL
#endif

// Disable asserts, no asserts lib.
#undef assert
#define assert(condition) ((void)0)

// XZ START

static inline unsigned int
read32le(const unsigned char *buf)
{
	unsigned int num = (unsigned int)buf[0];
	num |= (unsigned int)buf[1] << 8;
	num |= (unsigned int)buf[2] << 16;
	num |= (unsigned int)buf[3] << 24;
	return num;
}

// The code currently assumes that size_t is either 32-bit or 64-bit.

#ifndef SIZE_MAX
#   if defined(__x86_64__) || defined(_M_X64) || defined(__aarch64__) || defined(_M_ARM64)
#       define SIZE_MAX 18446744073709551615ULL
#   elif defined(__i386__) || defined(_M_IX86) || defined(__arm__)
#       define SIZE_MAX 4294967295U
#   else
#       error Size of size_t could not be determined natively
#   endif
#endif

#	ifndef lzma_attr_warn_unused_result
#		if __GNUC__ == 3 && __GNUC_MINOR__ < 4
#			define lzma_attr_warn_unused_result
#		endif
#	endif

#ifndef lzma_attr_warn_unused_result
#	define lzma_attr_warn_unused_result \
		lzma_attribute((__warn_unused_result__))
#endif

#ifndef lzma_attr_pure
#	define lzma_attr_pure lzma_attribute((__pure__))
#endif

#ifndef lzma_attr_const
#	define lzma_attr_const lzma_attribute((__const__))
#endif

#	ifndef conv32le
#		define conv32le(num) ((uint32_t)(num))
#	endif

#ifndef ARRAY_SIZE
#	define ARRAY_SIZE(array) (sizeof(array) / sizeof((array)[0]))
#endif

#if defined(__GNUC__) && defined(__GNUC_MINOR__)
#	define TUKLIB_GNUC_REQ(major, minor) \
		((__GNUC__ == (major) && __GNUC_MINOR__ >= (minor)) \
			|| __GNUC__ > (major))
#else
#	define TUKLIB_GNUC_REQ(major, minor) 0
#endif

// Calls the given function once. This isn't thread safe.
#define mythread_once(func) \
do { \
	static bool once_ = false; \
	if (!once_) { \
		func(); \
		once_ = true; \
	} \
} while (0)

#undef memzero
#define memzero(s, n) memset(s, 0, n)

#define my_min(x, y) ((x) < (y) ? (x) : (y))
#define my_max(x, y) ((x) > (y) ? (x) : (y))

#if defined(__GNUC__) \
		&& ((__GNUC__ == 4 && __GNUC_MINOR__ >= 3) || __GNUC__ > 4)
#	define lzma_attr_alloc_size(x) __attribute__((__alloc_size__(x)))
#else
#	define lzma_attr_alloc_size(x)
#endif

#if __STDC_VERSION__ >= 202311
#	define FALLTHROUGH [[__fallthrough__]]
#elif (defined(__GNUC__) && __GNUC__ >= 7) \
		|| (defined(__clang_major__) && __clang_major__ >= 10)
#	define FALLTHROUGH __attribute__((__fallthrough__))
#else
#	define FALLTHROUGH ((void)0)
#endif

#ifndef LZMA_API_CALL
#   if defined(_WIN32) && !defined(__CYGWIN__) && !defined(__x86_64__) && !defined(_M_X64)
#       define LZMA_API_CALL __stdcall
#   else
#       define LZMA_API_CALL
#   endif
#endif
#ifndef LZMA_API_IMPORT
#   define LZMA_API_IMPORT
#endif

#if defined(_WIN32) || defined(__CYGWIN__)
#	ifdef DLL_EXPORT
#		define LZMA_API_EXPORT __declspec(dllexport)
#	else
#		define LZMA_API_EXPORT
#	endif
#	define lzma_attr_visibility_hidden
// Don't use ifdef or defined() below.
#elif HAVE_VISIBILITY
#	define LZMA_API_EXPORT __attribute__((__visibility__("default")))
#	define lzma_attr_visibility_hidden \
			__attribute__((__visibility__("hidden")))
#else
#	define LZMA_API_EXPORT
#	define lzma_attr_visibility_hidden
#endif

// XZ END

#define LZMA_API(type) LZMA_API_EXPORT type LZMA_API_CALL

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

#if defined(__x86_64__) || defined(_M_X64) || defined(__aarch64__) || defined(_M_ARM64)
    // 64-bit Systems: Pointers are 8 bytes, matching unsigned long long
    typedef unsigned long long uintptr_t;
    typedef signed long long   intptr_t;
#else
    // 32-bit Systems: Pointers are 4 bytes, matching unsigned int
    typedef unsigned int       uintptr_t;
    typedef signed int         intptr_t;
#endif

#ifdef __cplusplus
extern "C" {
#endif

__declspec(dllimport) void* __stdcall GetProcessHeap(void);
__declspec(dllimport) void* __stdcall HeapAlloc(void* hHeap, unsigned long dwFlags, size_t dwBytes);
__declspec(dllimport) void* __stdcall HeapReAlloc(void* hHeap, unsigned long dwFlags, void* lpMem, size_t dwBytes);
__declspec(dllimport) int   __stdcall HeapFree(void* hHeap, unsigned long dwFlags, void* lpMem);

#ifdef __cplusplus
}
#endif

// Win32 Heap Flag allocation constant for clearing bytes

#define HEAP_ZERO_MEMORY 0x00000008

void* malloc(size_t size);
void* calloc(size_t num, size_t size);
void* realloc(void* ptr, size_t size);
void free(void* ptr);
int memcmp(const void* buf1, const void* buf2, size_t count);
void* memcpy(void* destination, const void* source, size_t count);
void* memset(void* dest, int value, size_t count);
void* memmove(void* destination, const void* source, size_t count);

#endif // GLUE_H