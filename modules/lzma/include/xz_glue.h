#pragma once

static inline unsigned int
read32le(const unsigned char *buf)
{
	unsigned int num = (unsigned int)buf[0];
	num |= (unsigned int)buf[1] << 8;
	num |= (unsigned int)buf[2] << 16;
	num |= (unsigned int)buf[3] << 24;
	return num;
}

/**
 * write32le: Writes a 32-bit integer to memory in Little-Endian format.
 * Works regardless of the host CPU's native endianness.
 */
static inline void write32le(void* buffer, uint32_t value) {
    uint8_t* p = (uint8_t*)buffer;

#if defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    // The host is Big-Endian: We MUST swap the bytes to store as Little-Endian.
    p[0] = (uint8_t)(value >> 24);
    p[1] = (uint8_t)(value >> 16);
    p[2] = (uint8_t)(value >> 8);
    p[3] = (uint8_t)(value);
#else
    // The host is Little-Endian (or unknown, defaulting to LE):
    // We can perform a direct write. The compiler will optimize this.
    *(uint32_t*)p = value;
#endif
}

/**
 * @brief Reads a 32-bit unsigned integer from an aligned memory address 
 * in native endian format.
 * * @param ptr Pointer to the data. Must be 4-byte aligned.
 * @return uint32_t The 32-bit value.
 */
static inline uint32_t aligned_read32ne(const void *ptr) {
    // Optional: Verify 4-byte alignment during debugging
    assert(((uintptr_t)ptr % 4) == 0); 
    
    return *(const uint32_t *)ptr;
}

#define LZMA_API(type) LZMA_API_EXPORT type LZMA_API_CALL

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

#ifndef UINT64_C
	#define UINT64_C(n) n ## ULL
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