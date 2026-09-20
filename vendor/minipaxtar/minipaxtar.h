#ifndef MINIPAXTAR_H
#define MINIPAXTAR_H

/**
 * \file minipaxtar.h
 * \brief Minimal PAX and USTAR TAR archive library.
 * \author Vulpi <78976654+imvulpi@users.noreply.github.com>
 * 
 * \copyright Copyright (c) 2026 Vulpi
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the “Software”), to deal 
 * in the Software without restriction, including without limitation the rights 
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell 
 * copies of the Software, and to permit persons to whom the Software is 
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE 
 * SOFTWARE.
 */

#ifdef MPTAR_NO_STD
    typedef unsigned char mptar_uint8;
    typedef signed char mptar_int8;
    
    #if defined(__INT_MAX__) && (__INT_MAX__ == 32767)
        typedef unsigned int mptar_uint16;
        typedef signed int mptar_int16;
    #else
        typedef unsigned short mptar_uint16;
        typedef signed short mptar_int16;
    #endif

    #if defined(__INT_MAX__) && (__INT_MAX__ == 2147483647)
        typedef unsigned int mptar_uint32;
        typedef signed int mptar_int32;
    #else
        typedef unsigned long mptar_uint32;
        typedef signed long mptar_int32;
    #endif

    typedef unsigned long long mptar_uint64;
    typedef signed long long mptar_int64;

    #if defined(__SIZEOF_POINTER__) && (__SIZEOF_POINTER__ == 8) || defined(_WIN64)
        typedef unsigned long long mptar_size_t;
    #elif defined(__SIZEOF_POINTER__) && (__SIZEOF_POINTER__ == 2) 
        typedef unsigned short mptar_size_t;
    #else
        typedef unsigned long mptar_size_t;
    #endif

    #define MPTAR_NULL ((void*)0)

    #define MPTAR_INT64_MAX  (9223372036854775807LL)
    #define MPTAR_INT64_MIN  (-MPTAR_INT64_MAX - 1LL)
    #define MPTAR_UINT64_MAX (0xFFFFFFFFFFFFFFFFULL)
    #define MPTAR_UINT32_MAX (4294967295U)
#ifndef bool
    typedef _Bool bool;
    #define true  1
    #define false 0
#endif

#else
    #include <stdint.h>
    #include <stddef.h>
    #include <stdbool.h>

    /**
     * \defgroup mptar_primitives Primitive Types and Limits
     * \brief Fundamental integer, size, floating-point types, and their scalar limits.
     * @{
     */
    typedef int64_t  mptar_int64;  /**< Signed 64-bit integer. */
    typedef int32_t  mptar_int32;  /**< Signed 32-bit integer. */
    typedef int16_t  mptar_int16;  /**< Signed 16-bit integer. */
    typedef int8_t   mptar_int8;   /**< Signed 8-bit integer. */

    #define MPTAR_NULL       NULL       /**< Null pointer constant. */

    typedef uint64_t mptar_uint64; /**< Unsigned 64-bit integer. */
    typedef uint32_t mptar_uint32; /**< Unsigned 32-bit integer. */
    typedef uint16_t mptar_uint16; /**< Unsigned 16-bit integer. */
    typedef uint8_t  mptar_uint8;  /**< Unsigned 8-bit integer. */
    typedef size_t   mptar_size_t; /**< Unsigned size type. */
    
    #define MPTAR_INT64_MIN  INT64_MIN  /**< Minimum value for \ref mptar_int64. */
    #define MPTAR_INT64_MAX  INT64_MAX  /**< Maximum value for \ref mptar_int64. */
    #define MPTAR_UINT64_MAX UINT64_MAX /**< Maximum value for \ref mptar_uint64. */
    #define MPTAR_UINT32_MAX UINT32_MAX /**< Maximum value for \ref mptar_uint32. */
    /** @} */
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Documentation for compile-time configuration macros.
 * \details The \c DOXYGEN_DOCS macro is used exclusively by documentation generators 
 *          like Doxygen. Because minipaxtar enables reading and writing by default, 
 *          these option macros (such as \c MPTAR_NO_READ) are not normally defined in the 
 *          source code. Wrapping them in \c "#ifdef DOXYGEN_DOCS" allows Doxygen to index 
 *          and document them without altering actual build configurations.
 */
#ifdef DOXYGEN_DOCS 
/**
 * \defgroup mptar_stable_config Stable Compile-Time Options
 * \brief Stable configuration macros and custom conversion/string hooks.
 * \details These options, features, and custom conversion hooks are guaranteed stable 
 *          and will follow standard deprecation cycles if ever slated for modification or removal.
 * \{
 */

#define MPTAR_NO_STD                /**< Disable standard library dependencies (-DMPTAR_NO_STD) for freestanding/bare-metal environments. */
#define MPTAR_NO_READ               /**< Disable read support (-DMPTAR_NO_READ) to strip archive reading functionality and reduce binary size. */
#define MPTAR_NO_WRITE              /**< Disable write support (-DMPTAR_NO_WRITE) to strip archive writing functionality and reduce binary size. */
#define MPTAR_SUPPORT_SPECIAL       /**< Enable support for special TAR entry types (-DMPTAR_SUPPORT_SPECIAL) like long links, PAX headers, and sparse files. */
#define MPTAR_SUPPORT_EXTRA_TIMES   /**< Enable extra timestamp support (-DMPTAR_SUPPORT_EXTRA_TIMES) to parse additional granular file times. */

#define MPTAR_CUSTOM_U64TOA         /**< Custom implementation override for unsigned 64-bit integer to string. */
#define MPTAR_CUSTOM_I64TOA         /**< Custom implementation override for signed 64-bit integer to string. */
#define MPTAR_CUSTOM_ATOU64         /**< Custom implementation override for string to unsigned 64-bit integer. */
#define MPTAR_CUSTOM_ATOI64         /**< Custom implementation override for string to signed 64-bit integer. */
#define MPTAR_CUSTOM_STRLEN         /**< Custom implementation override for string length calculation. */
#define MPTAR_CUSTOM_STRNLEN        /**< Custom implementation override for bounded string length calculation. */
#define MPTAR_CUSTOM_STRNCPY        /**< Custom implementation override for bounded string copying. */
#define MPTAR_CUSTOM_MEMCPY         /**< Custom implementation override for memory block copying. */
#define MPTAR_CUSTOM_MEMSET         /**< Custom implementation override for memory block setting. */
#define MPTAR_CUSTOM_MEMCMP         /**< Custom implementation override for memory block comparison. */

/** \} */

/**
 * \defgroup mptar_experimental_config Experimental & Internal Overrides
 * \warning **Volatile API / No Stability Guarantees:** These internal macros and low-level hooks 
 *          are subject to change or complete removal without notice in future versions. 
 *          Use them at your own risk.
 * \brief Unstable low-level hooks and internal customization switches.
 * \details All \c MPTAR_CUSTOM_* hooks are related to internal functions inside \c minipaxtar.c. 
 *          Unless explicitly listed under \ref mptar_stable_config, these custom functions 
 *          are considered internal and carry no stability guarantees.
 * \{
 */

#define MPTAR_CUSTOM_INTERNAL_HOOK_EXAMPLE /**< General catch-all representation for internal or undocumented custom hooks. */

/** \} */
#endif

/**
 * \defgroup mptar_status_codes Status and Return Codes
 * \brief Non-negative codes indicating successful operations, streams conditions, or encoding requirements.
 * @{
 */
#define MPTAR_OK                        0   /**< Operation completed successfully. */
#define MPTAR_EOF                       1   /**< End of Archive reached (two consecutive null blocks). */
#define MPTAR_NEEDS_PAX                 2   /**< Item cannot fit in standard USTAR and requires a PAX extended header. */
/** @} */

/**
 * \defgroup mptar_error_codes Error Codes
 * \brief Negative error codes indicating some failure, returned by minipaxtar API functions.
 * @{
 */
#define MPTAR_ERR_INVALID_ARG          -1   /**< NULL context pointers or zero-length arguments passed to an API function. */
#define MPTAR_ERR_ALLOC                -2   /**< Memory allocation failed or returned an invalid pointer. */
#define MPTAR_ERR_IO_READ              -3   /**< Read callback failed to return the requested number of bytes (unexpected end of stream). */
#define MPTAR_ERR_IO_WRITE             -4   /**< Write callback failed to write the complete chunk to storage. */
#define MPTAR_ERR_CHECKSUM             -5   /**< Header checksum verification failed; record is corrupted or misaligned. */
/**
 * \brief Encountered a special file type flag (FIFO, Block, Char device).
 * \note Returning this error code usually indicates that special file support 
 *       was disabled at compile time via \c MPTAR_SUPPORT_SPECIAL.
 */
#define MPTAR_ERR_UNSUPPORTED_TYPE     -6
#define MPTAR_ERR_RESERVED_LEGACY_1    -7   /**< Reserved / Legacy. Formerly used for write overflow prior to automatic payload clamping. */
#define MPTAR_ERR_INCOMPLETE_PAYLOAD   -8   /**< Called mptar_write_finalize() or attempted to start a new header while bytes_left > 0 on the current file payload. */
#define MPTAR_ERR_MALFORMED            -9   /**< Archive structural corruption or malformed header formatting detected (e.g., invalid octal fields, mangled PAX key-value pairs). */
#define MPTAR_ERR_OVERFLOW             -10  /**< An arithmetic wrap-around or internal buffer write boundary violation detected. */
#define MPTAR_ERR_RESERVED_LEGACY_2    -11  /**< Reserved / Legacy. Formerly used to signal PAX requirement due to a long path. */
#define MPTAR_ERR_NS_CONVERSION_FAILED -12  /**< Conversion of sub-second nanoseconds to string failed (nanosecond value exceeded 999,999,999). */
#define MPTAR_ERR_BUFFER_TOO_SMALL     -13  /**< Output or destination buffer lacked sufficient capacity to hold some data. */
/** @} */

/**
 * \defgroup mptar_pax_overhead PAX Fixed Keywords & Static Overhead
 * \brief Constants defining raw keyword sizes and structural character overhead for PAX records.
 * @{
 */
#define MPTAR_PAX_KEY_LEN_PATH            4    /**< Length of "path" keyword string. */
#define MPTAR_PAX_KEY_LEN_SIZE            4    /**< Length of "size" keyword string. */
#define MPTAR_PAX_KEY_LEN_LINK            8    /**< Length of "linkpath" keyword string. */
#define MPTAR_PAX_KEY_LEN_TIME            5    /**< Length of "mtime", "ctime", "atime" keywords string. */
#define MPTAR_PAX_KEY_LEN_UID             3    /**< Length of "uid" keyword string. */
#define MPTAR_PAX_KEY_LEN_GID             3    /**< Length of "gid" keyword string. */
#define MPTAR_PAX_KEY_LEN_UNAME           5    /**< Length of "uname" keyword string. */
#define MPTAR_PAX_KEY_LEN_GNAME           5    /**< Length of "gname" keyword string. */

#define MPTAR_PAX_STATIC_CHARS            3    /**< Fixed character count per record: space (' '), equals ('='), and newline ('\n'). */

#define MPTAR_PAX_OVERHEAD_PATH           (MPTAR_PAX_KEY_LEN_PATH + MPTAR_PAX_STATIC_CHARS)   /**< Total key+formatting overhead for "path" record (7 bytes). */
#define MPTAR_PAX_OVERHEAD_SIZE           (MPTAR_PAX_KEY_LEN_SIZE + MPTAR_PAX_STATIC_CHARS)   /**< Total key+formatting overhead for "size" record (7 bytes). */
#define MPTAR_PAX_OVERHEAD_LINK           (MPTAR_PAX_KEY_LEN_LINK + MPTAR_PAX_STATIC_CHARS)   /**< Total key+formatting overhead for "linkpath" record (11 bytes). */
#define MPTAR_PAX_OVERHEAD_TIME           (MPTAR_PAX_KEY_LEN_TIME + MPTAR_PAX_STATIC_CHARS)   /**< Total key+formatting overhead for "mtime" record (8 bytes). */
#define MPTAR_PAX_OVERHEAD_UID            (MPTAR_PAX_KEY_LEN_UID   + MPTAR_PAX_STATIC_CHARS)  /**< Total key+formatting overhead for "uid" record (6 bytes). */
#define MPTAR_PAX_OVERHEAD_GID            (MPTAR_PAX_KEY_LEN_GID   + MPTAR_PAX_STATIC_CHARS)  /**< Total key+formatting overhead for "gid" record (6 bytes). */
#define MPTAR_PAX_OVERHEAD_UNAME          (MPTAR_PAX_KEY_LEN_UNAME + MPTAR_PAX_STATIC_CHARS)  /**< Total key+formatting overhead for "uname" record (8 bytes). */
#define MPTAR_PAX_OVERHEAD_GNAME          (MPTAR_PAX_KEY_LEN_GNAME + MPTAR_PAX_STATIC_CHARS)  /**< Total key+formatting overhead for "gname" record (8 bytes). */
#define MPTAR_PAX_NSEC_OVERHEAD           2U   /**< Additional bytes for sub-second formatting: '.' delimiter + '\0' null terminator. */
/** @} */

/**
 * \defgroup mptar_format_limits USTAR & PAX Field Capacities, Bounds, and Buffer Limits
 * \brief Fixed array sizes, maximum string lengths, encoding numeric limits, and formatting buffer sizes.
 * @{
 */

/* Raw Header Field Capacities */
#define MPTAR_USTAR_SIZE_LINKNAME         100  /**< Raw size of the header linkname field in bytes. */
#define MPTAR_USTAR_SIZE_NAME             100  /**< Raw size of the header filename field in bytes. */
#define MPTAR_USTAR_SIZE_PREFIX           155  /**< Raw size of the header prefix field in bytes. */
#define MPTAR_USTAR_SIZE_UNAME            32   /**< Raw size of the header owner user name field in bytes. */
#define MPTAR_USTAR_SIZE_GNAME            32   /**< Raw size of the header owner group name field in bytes. */

/* Maximum String Lengths (excluding null terminators) */
#define MPTAR_USTAR_MAX_LEN_NAME          (MPTAR_USTAR_SIZE_NAME - 1)     /**< Maximum string length for filename (99 chars). */
#define MPTAR_USTAR_MAX_LEN_LINKNAME      (MPTAR_USTAR_SIZE_LINKNAME - 1) /**< Maximum string length for linkname (99 chars). */
#define MPTAR_USTAR_MAX_LEN_PREFIX        (MPTAR_USTAR_SIZE_PREFIX - 1)   /**< Maximum string length for prefix (154 chars). */
#define MPTAR_USTAR_MAX_LEN_UNAME         (MPTAR_USTAR_SIZE_UNAME - 1)    /**< Maximum string length for uname (31 chars). */
#define MPTAR_USTAR_MAX_LEN_GNAME         (MPTAR_USTAR_SIZE_GNAME - 1)    /**< Maximum string length for gname (31 chars). */

/* Encoding Numeric Bounds & Formatting Limits */
#define MPTAR_USTAR_MAX_OCTAL_12B         8589934591ULL     /**< Maximum value encodeable in a 12-byte USTAR octal field (77777777777 octal). */
#define MPTAR_USTAR_MAX_OCTAL_8B          2097151ULL        /**< Maximum value encodeable in an 8-byte USTAR octal field (7777777 octal). */
#define MPTAR_MAX_OCTAL_DIGITS_64         22U               /**< Max octal digits required to represent a 64-bit integer. */
#define MPTAR_BINARY_MAX_8B               MPTAR_INT64_MAX   /**< Max boundary for 8-byte base-256 binary encoded values. */
#define MPTAR_PAX_MAX_UINT32_STR_LEN      10U               /**< String length required to represent a uint32 value when its value exceeds 999999999 (10 digits, up to 4294967295). */
#define MPTAR_PAX_NSEC_MAX_DIGITS         9U                /**< Maximum digits needed for writing nanoseconds precision (10^-9). */
#define MPTAR_PAX_NSEC_INITIAL_MULTIPLIER 100000000U        /**< Scale factor (10^8) for extracting top nanosecond digit. */
#define MPTAR_PAX_MAX_DATA_LEN            (MPTAR_UINT32_MAX - MPTAR_PAX_MAX_UINT32_STR_LEN) /**< Maximum payload byte capacity inside a single PAX record. */

/* Formatting Buffer Sizes */
#define MPTAR_UINT64_STR_BUF_SIZE         24U               /**< Buffer capacity used to safely format a 64-bit uint string (including terminator). */
#define MPTAR_TIMESPEC_STR_BUF_SIZE       32U               /**< Buffer capacity used to safely format full PAX timestamp string ("sec.nsec", including null terminator). */

#ifndef MPTAR_MAX_PAX_SIZE
/**
 * \brief Maximum permitted byte size for a single PAX extended header payload.
 * \details Protects against Denial of Service (DoS) attacks or dynamic allocation panics 
 *          caused by corrupted or untrusted TAR archives with unnaturally large PAX size headers.
 *          Defaults to 8 MiB (8,388,608 bytes). Can be overridden at compile-time.
 */
#define MPTAR_MAX_PAX_SIZE (8 * 1024 * 1024)
#endif

/** @} */

/**
 * \defgroup mptar_format_misc Archive Format Misc & Block Layout Constants
 * \brief Fundamental block sizes, binary encoding flags, checksum field offsets, and scaling factors.
 * @{
 */
#define MPTAR_OCTAL_BINARY_FLAG           0x80U     /**< High bit flag set in octal fields to indicate base-256 binary encoding. */
#define MPTAR_USTAR_HEADER_SIZE           512       /**< Byte size of a standard USTAR header block. */
#define MPTAR_BLOCK_SIZE                  512       /**< Standard block layout size for tar archives. */
#define MPTAR_SPACE_CHECKSUM_VAL          256       /**< Pre-calculated checksum baseline for an empty checksum field (8 ASCII spaces). */
#define MPTAR_CHECKSUM_OFFSET             148U      /**< Byte offset of the checksum field within the standard 512-byte header. */
#define MPTAR_CHECKSUM_LEN                8U        /**< Byte length of the checksum field inside the header. */
#define MPTAR_CHECKSUM_END_OFFSET         (MPTAR_CHECKSUM_OFFSET + MPTAR_CHECKSUM_LEN) /**< End offset of the checksum field (156). */

#define MPTAR_EOA_BLOCK_COUNT             2U        /**< Number of trailing zero-filled blocks designating End-of-Archive (EOA). */
#define MPTAR_EOA_MARKER_SIZE             (MPTAR_EOA_BLOCK_COUNT * MPTAR_BLOCK_SIZE) /**< Total marker size for archive termination (1024 bytes). */
/** @} */

/**
 * \defgroup mptar_pax_flags PAX Record Tracking Flags
 * \brief Bit flags tracking successfully parsed fields from a PAX extended header.
 * \note These flags indicate that a PAX keyword was both present in the block 
 *       and successfully parsed into \ref mptar_metadata. If keyword parsing fails, 
 *       the respective bit is explicitly left unset.
 * @{
 */
#define MPTAR_PAX_HAS_PATH                (1 << 0) /**< Bit set if a path keyword existed in the pax entry and parsing the path was successful. */
#define MPTAR_PAX_HAS_LINK                (1 << 1) /**< Bit set if a linkpath keyword existed in the pax entry and parsing the linkpath was successful. */
#define MPTAR_PAX_HAS_SIZE                (1 << 2) /**< Bit set if a size keyword existed in the pax entry and parsing the size was successful. */
#define MPTAR_PAX_HAS_MTIME               (1 << 3) /**< Bit set if an mtime keyword existed in the pax entry and parsing the mtime was successful. */
#define MPTAR_PAX_HAS_UID                 (1 << 4) /**< Bit set if a uid keyword existed in the pax entry and parsing the uid was successful. */
#define MPTAR_PAX_HAS_GID                 (1 << 5) /**< Bit set if a gid keyword existed in the pax entry and parsing the gid was successful. */
#define MPTAR_PAX_HAS_ATIME               (1 << 6) /**< Bit set if an atime keyword existed in the pax entry and parsing the atime was successful. */
#define MPTAR_PAX_HAS_CTIME               (1 << 7) /**< Bit set if a ctime keyword existed in the pax entry and parsing the ctime was successful. */
#define MPTAR_PAX_HAS_UNAME               (1 << 8) /**< Bit set if a uname keyword existed in the pax entry and parsing the uname was successful. */
#define MPTAR_PAX_HAS_GNAME               (1 << 9) /**< Bit set if a gname keyword existed in the pax entry and parsing the gname was successful. */
/** @} */


/**
 * \defgroup mptar_writer_flags Writer Flags
 * \brief Control flags that alter writing behavior and fallback logic.
 * @{
 */

/**
 * \brief Context flag to prefer PAX extended headers over base-256 binary encoding for large octal fields.
 * \note When an archive field value exceeds standard USTAR octal capacity, mptar
 *       normally encodes it using a base-256 binary representation (setting the high-order bit via 
 *       \ref MPTAR_OCTAL_BINARY_FLAG). 
 *       \n\n
 *       If this flag is set, PAX extended headers are emitted alongside binary encoding whenever 
 *       binary encoding is available. Setting this flag does *not* prevent binary encoding from being used; 
 *       rather, it allows PAX entries to be included in addition to it.
 *       \n\n
 *       If binary encoding is not possible because the value is too large to fit in a 
 *       64-bit integer (exceeds \ref MPTAR_INT64_MAX), a PAX entry is included automatically 
 *       regardless of whether this flag is set. By default, this flag is unset, meaning PAX records 
 *       are omitted when binary encoding can be used.
 */
#define MPTAR_CTX_ALLOW_PAX_FOR_OCTAL     (1U << 0)
/** @} */

/**
 * \defgroup mptar_default_permissions Default File Permissions
 * \brief Standard UNIX permission masks used when creating entries.
 * @{
 */
#define MPTAR_MODE_DIR                    493  /**< Default directory permissions: 0755 (rwxr-xr-x). */
#define MPTAR_MODE_REG                    420  /**< Default regular file permissions: 0644 (rw-r--r--). */
/** @} */

/**
 * \defgroup mptar_data_structure Archive Data Structures
 * \brief Representations for raw 512-byte USTAR header blocks, timestamps, and parsed entry metadata.
 * @{
 */

/**
 * \brief Raw 512-byte POSIX USTAR header layout matching the exact on-disk binary format.
 * \note All octal ASCII fields (such as size, mtime, uid, gid, etc.) can optionally use base-256 binary 
 *       encoding by setting the high-order bit (\ref MPTAR_OCTAL_BINARY_FLAG) in the first byte, 
 *       with the explicit exception of the checksum field, which must always remain in octal ASCII.
 */
typedef struct {
    char name[100];     /**< File path or filename (null-terminated if under 100 bytes). */
    char mode[8];       /**< File permissions in octal ASCII or base-256 binary. */
    char uid[8];        /**< Owner user ID in octal ASCII or base-256 binary. */
    char gid[8];        /**< Owner group ID in octal ASCII or base-256 binary. */
    char size[12];      /**< File payload size in octal ASCII or base-256 binary. */
    char mtime[12];     /**< Modification time (Unix epoch seconds) in octal ASCII or base-256 binary. */
    
    /**
     * \brief Header checksum in octal ASCII.
     * \note Calculated by treating these 8 bytes as ASCII spaces, summing all 512 bytes 
     *       of the header as unsigned bytes, and storing the result as an octal string 
     *       followed by a null and a space. Binary encoding is strictly forbidden here.
     */
    char checksum[8];   

    /**
     * \brief File entry type flag.
     * \note Standard values include: 
     *   - \c '0' or \c '\0': Regular file 
     *   - \c '1': Hard link
     *   - \c '2': Symbolic link
     *   - \c '3': Character special file
     *   - \c '4': Block special file
     *   - \c '5': Directory
     *   - \c '6': FIFO / pipe
     *   - \c '7': Contiguous file
     *   - \c 'x': PAX extended header
     *   - \c 'g': PAX global extended header
     */
    char typeflag;
    char linkname[100]; /**< Target path for symbolic or hard links. */
    char magic[6];      /**< USTAR magic indicator ("ustar" or "ustar\0"). */
    char version[2];    /**< USTAR version string ("00"). */
    char uname[32];     /**< Owner user name string. */
    char gname[32];     /**< Owner group name string. */
    char devmajor[8];   /**< Major device number for character/block special files in octal ASCII or base-256 binary. */
    char devminor[8];   /**< Minor device number for character/block special files in octal ASCII or base-256 binary. */
    char prefix[155];   /**< Path prefix used to extend file paths up to 256 total characters. */
    char padding[12];   /**< Zero-padding bytes to complete the 512-byte block boundary. */
} mptar_header;

/**
 * \brief High-resolution timestamp holding Unix epoch seconds and sub-second nanoseconds.
 * \note Negative epoch seconds are supported via \ref mptar_int64, but writing a negative 
 *       timestamp forces the emission of a PAX extended header because standard USTAR formats 
 *       cannot natively represent negative time values.
 */
typedef struct {
    /**
     * \brief Standard Unix timestamp in seconds.
     * \note Supports negative epoch values. Writing a negative timestamp forces 
     *       the emission of a PAX extended header because standard USTAR headers 
     *       cannot natively represent negative time values.
     */
    mptar_int64 sec;

    /**
     * \brief Sub-second precision in nanoseconds.
     * \note Must not exceed 999,999,999 when writing, or an error will occur. 
     *       Attempting to write an invalid nanosecond value triggers a specific error: 
     *       \ref MPTAR_ERR_NS_CONVERSION_FAILED.
     */
    mptar_uint32 nsec;
} mptar_timespec;

/**
 * \brief Optional timestamp container. 
 */
typedef struct {
    mptar_timespec value; /**< High-resolution timestamp value. */
    bool has_value;       /**< True if the timestamp is present and valid. */
} mptar_opt_time;

/**
 * \brief Universal, high-level archive entry metadata.
 * \note Unlike the fixed-size on-disk \ref mptar_header, this structure uses spacious 
 *       data types (such as 64-bit integers for IDs and sizes) and supports unbounded strings 
 *       (like fully unconstrained paths and usernames). Time values like \ref mtime support 
 *       sub-second nanosecond precision and negative epochs.
 */
typedef struct {
    mptar_opt_time mtime;     /**< File modification time with optional sub-second precision. */
#ifdef MPTAR_SUPPORT_EXTRA_TIMES
    mptar_opt_time atime;     /**< File access time (used in a PAX extended header). */
    mptar_opt_time ctime;     /**< File metadata change time (used in a PAX extended header). */
#endif

    mptar_uint64 size;        /**< Payload size in bytes. */
    mptar_uint64 uid;         /**< Owner user ID. */
    mptar_uint64 gid;         /**< Owner group ID. */

    const char* path;         /**< Full file path string (null-terminated). */
    const char* link_target;  /**< Symlink/hardlink target path string (null-terminated). */
    const char* uname;        /**< Owner user name string (null-terminated). */
    const char* gname;        /**< Owner group name string (null-terminated). */

    mptar_uint32 mode;        /**< UNIX file permission bitmask. */
#ifdef MPTAR_SUPPORT_SPECIAL
    mptar_uint32 devminor;    /**< Minor device number for special files. */
    mptar_uint32 devmajor;    /**< Major device number for special files. */
#endif

    /**
     * \brief File entry type flag.
     * \note Standard values include: 
     *   - \c '0' or \c '\0': Regular file 
     *   - \c '1': Hard link
     *   - \c '2': Symbolic link
     *   - \c '3': Character special file
     *   - \c '4': Block special file
     *   - \c '5': Directory
     *   - \c '6': FIFO / pipe
     *   - \c '7': Contiguous file
     *   - \c 'x': PAX extended header
     *   - \c 'g': PAX global extended header
     */
    char typeflag;
    bool internal_alloc;      /**< True if string fields were heap-allocated internally by reader. */
} mptar_metadata;

/** @} */

/**
 * \defgroup mptar_memory Memory Management Callbacks and Configuration
 * \brief Custom memory allocation hook definitions and configuration context.
 * @{
 */

/**
 * \brief Custom memory allocation function signature.
 * \param user_data Opaque pointer passed through from \ref mptar_memory_cfg.
 * \param size Number of bytes to allocate.
 * \return Pointer to allocated memory, or NULL on failure.
 */
typedef void* (*mptar_alloc_fn)(void* user_data, mptar_size_t size);

/**
 * \brief Custom memory deallocation function signature.
 * \param user_data Opaque pointer passed through from \ref mptar_memory_cfg.
 * \param ptr Pointer to the memory block to free.
 */
typedef void (*mptar_free_fn)(void* user_data, void* ptr);

/**
 * \brief Custom allocator configuration structure.
 */
typedef struct {
    mptar_alloc_fn alloc;    /**< Pointer to custom allocation function. */
    mptar_free_fn free;      /**< Pointer to custom deallocation function. */
    void* alloc_user_data;   /**< User-provided opaque pointer that is passed directly to alloc/free. */
} mptar_memory_cfg;

/** @} */

#ifndef MPTAR_NO_READ
/**
 * \defgroup mptar_reader Reader Subsystem
 * \brief Reader callbacks, stream context, and entry extraction functions.
 * @{
 */

/**
 * \brief Read stream callback signature.
 * \param user_data Opaque stream context pointer.
 * \param buffer Output buffer to receive read bytes.
 * \param size Maximum number of bytes to read.
 * \return Total number of bytes actually read, or 0 on error/EOF.
 */
typedef mptar_size_t (*mptar_read_fn)(void* user_data, void* buffer, mptar_size_t size);

/**
 * \brief Archive reader state and context structure.
 */
typedef struct {
    mptar_uint64 bytes_left;  /**< Remaining unread payload bytes in current file entry. */
    mptar_uint64 offset;      /**< Total byte offset processed within stream. */
    mptar_memory_cfg memory;  /**< Memory allocator configuration context. */
    mptar_read_fn read;       /**< I/O stream read callback function. */
    void* read_user_data;     /**< Opaque user context pointer passed to read callback. */
} mptar_reader;

/**
 * \brief Reads and parses the next header entry in the tar archive.
 * \details Iterates through the input stream handling standard USTAR header blocks, PAX extended attributes, 
 *          and unhandled extension records, populating the destination metadata structure with parsed file 
 *          attributes, paths, and allocation configurations.
 * \param reader Pointer to initialized \ref mptar_reader context.
 * \param out_meta Output structure populated with entry \ref mptar_metadata.
 * \return \ref MPTAR_OK on success, \ref MPTAR_EOF if the end-of-archive marker is encountered, or a negative error code.
 */
int mptar_read_header(mptar_reader* reader, mptar_metadata* out_meta);

/**
 * \brief Reads a slice of current tar file entry payload data into a buffer.
 * \details Validates reader state, ensures proper block stream alignment, clamps read requests 
 *          to remaining entry payload lengths, and reads data via the underlying callback reader.
 * \note Must be called after a header has been successfully read, as reading a header 
 *       initializes the necessary remaining payload byte counter (\c bytes_left) for the current entry.
 * \param reader Pointer to initialized \ref mptar_reader context.
 * \param buffer Output buffer to copy payload data.
 * \param size Maximum capacity of target buffer in bytes.
 * \param[out] out_err Output status pointer receiving error code on failure (e.g. \ref MPTAR_ERR_IO_READ).
 * \return Total number of payload bytes successfully written into buffer.
 */
mptar_size_t mptar_read_data_chunk(mptar_reader* reader, void* buffer, mptar_size_t size, int *out_err);

/**
 * \brief Discards data by reading the remaining payload bytes in the current tar entry stream.
 * \details Actively reads and discards any remaining unread bytes of the active entry from the input stream 
 *          and synchronizes block alignment via active stream discarding.
 * \note After returning \ref MPTAR_OK, the remaining payload byte counter (\c bytes_left) is guaranteed to be 0.
 * \param reader Pointer to initialized \ref mptar_reader context.
 * \return \ref MPTAR_OK on success, or a negative error code (e.g., \ref MPTAR_ERR_INVALID_ARG or I/O read errors).
 */
int mptar_discard_data(mptar_reader* reader);

/**
 * \brief Directly aligns the reader to the next 512-byte header boundary by updating stream offsets and resetting bytes_left to 0 
 *        without actually performing reads.
 * \note Call this function if you read or processed payload bytes manually without using \ref mptar_read_data_chunk.
 * \warning This function does not actually read from or advance the underlying stream; it only adjusts internal tracking counters, 
 *          assuming the user has already externally consumed the payload bytes.
 * \param reader Pointer to initialized \ref mptar_reader context.
 * \return \ref MPTAR_OK on success, or \ref MPTAR_ERR_INVALID_ARG if the reader pointer is invalid.
 */
int mptar_skip_data(mptar_reader* reader);

/**
 * \brief Frees internal heap resources allocated inside a metadata structure during header parsing.
 * \details Releases allocated memory for path, link target, user name, and group name strings 
 *          using the reader's configured memory allocator if they were internally allocated.
 * \warning Since \ref mptar_metadata can be shared or reused, this should not be called on metadata 
 *          that was not allocated by the reader. Although calling it on unmanaged metadata is generally 
 *          guarded internally by checking \c !meta->internal_alloc (which indicates whether the memory 
 *          was dynamically allocated by the library), doing so should be avoided.
 * \param reader Pointer to initialized \ref mptar_reader context containing memory configuration.
 * \param meta Pointer to metadata \ref mptar_metadata to clean up.
 */
void mptar_reader_free_metadata(mptar_reader* reader, mptar_metadata* meta);

/** @} */
#endif /* MPTAR_NO_READ */

#ifndef MPTAR_NO_WRITE
/**
 * \defgroup mptar_writer Writer Subsystem
 * \brief Writer callbacks, stream context, and archive creation functions.
 * @{
 */

/**
 * \brief Write stream callback signature.
 * \param user_data Opaque stream context pointer.
 * \param buffer Buffer containing data bytes to write out.
 * \param size Total number of bytes to write.
 * \return Total number of bytes written, or 0 on error.
 */
typedef mptar_size_t (*mptar_write_fn)(void* user_data, const void* buffer, mptar_size_t size);

/**
 * \brief Archive writer state and context structure.
 * \note The remaining payload byte counter (\ref bytes_left) must be 0 before finalizing an entry; 
 *       attempting to finalize while bytes remain will trigger \ref MPTAR_ERR_INCOMPLETE_PAYLOAD.
 */
typedef struct {
    mptar_uint64 bytes_left;  /**< Expected remaining payload bytes left to write for current entry (must reach 0 before finalization). */
    mptar_memory_cfg memory;  /**< Memory allocator configuration context. */
    mptar_write_fn write;     /**< I/O stream write callback function. */
    void* write_user_data;    /**< Opaque user context pointer passed to write callback. */
    mptar_uint32 flags;       /**< Bitmask of writer flags (e.g. \ref MPTAR_CTX_ALLOW_PAX_FOR_OCTAL). */
} mptar_writer;

/**
 * \brief Formats and emits a new header entry to the output stream.
 * \details Validates the provided metadata parameters, checks whether any fields exceed standard 
 *          POSIX USTAR boundary limits (such as paths, sizes, large timestamps, or high user/group IDs), 
 *          and generates either a standard USTAR header or both a PAX extended header and a USTAR header.
 * \note This function also updates the writer's internal state to match the provided metadata. 
 *       For example, if the metadata specifies a file payload size of 10 MB, the writer's 
 *       \ref mptar_writer::bytes_left counter will be initialized to that value.
 * \param ctx Pointer to initialized \ref mptar_writer context.
 * \param meta Pointer to the \ref mptar_metadata structure of am entry to create.
 * \return \ref MPTAR_OK on success, or a negative error code (e.g., \ref MPTAR_ERR_INVALID_ARG, \ref MPTAR_ERR_INCOMPLETE_PAYLOAD, \ref MPTAR_ERR_UNSUPPORTED_TYPE, \ref MPTAR_ERR_ALLOC, or \ref MPTAR_ERR_IO_WRITE).
 */
int mptar_write_header(mptar_writer* ctx, const mptar_metadata* meta);

/**
 * \brief Streams a payload chunk for the current entry out to storage.
 * \details Validates the available data capacity remaining for the current entry (`bytes_left`), 
 *          clamps the write size if it exceeds the expected entry length, and transmits the bytes 
 *          via the writer's underlying I/O stream callback.
 * \warning The output error pointer (\c out_err) should always be checked. If a write fails 
 *          or writes fewer bytes than requested, the underlying stream can become corrupted. 
 * \param ctx Pointer to initialized \ref mptar_writer context.
 * \param buffer Buffer holding payload data to write.
 * \param size Number of bytes in input buffer to write.
 * \param[out] out_err Output status pointer receiving error code on failure (e.g., \ref MPTAR_OK or \ref MPTAR_ERR_INVALID_ARG).
 * \return Number of payload bytes actually accepted and written out.
 */
mptar_size_t mptar_write_data_chunk(mptar_writer* ctx, const void* buffer, mptar_size_t size, int *out_err);

/**
 * \brief Finalizes current entry payload writing by calculating and emitting 512-byte alignment padding.
 * \details Checks if all promised payload bytes (`meta->size`) have been written. If `ctx->bytes_left == 0`,
 *          pads out the stream to the nearest 512-byte boundary using zero-bytes.
 * \param ctx Pointer to initialized \ref mptar_writer context.
 * \param meta Pointer to entry \ref mptar_metadata containing the total payload size.
 * \return \ref MPTAR_OK on success, or \ref MPTAR_ERR_INCOMPLETE_PAYLOAD if `ctx->bytes_left > 0`.
 */
int mptar_write_finalize(mptar_writer* ctx, const mptar_metadata* meta);

/**
 * \brief Writes standard POSIX End-of-Archive (EOA) zero blocks to properly terminate the archive stream.
 * \details Appends the standard tar end-of-archive marker blocks (two consecutive 512-byte blocks of zeroes) 
 *          to properly terminate the archive data stream.
 * \param ctx Pointer to initialized writer context.
 * \return \ref MPTAR_OK on success, or a negative error code.
 */
int mptar_close_archive(mptar_writer *ctx);

/** @} */
#endif /* MPTAR_NO_WRITE */

/**
 * \defgroup mptar_internal_diagnostics Internal Compiler
 * \brief Cross-compiler warning/diagnostics suppression and platform-abstraction macros.
 * @{
 */

#if defined(__clang__)
    /**
     * \brief Push compiler diagnostic state and suppress `-Wcast-qual` warnings.
     * \details Used when casting away `const` qualifiers in low-level buffer manipulation routines.
     */
    #define MPTAR_SUPPRESS_WARNING_CAST_QUAL_BEGIN \
        _Pragma("clang diagnostic push") \
        _Pragma("clang diagnostic ignored \"-Wcast-qual\"")

    /**
     * \brief Restore previous compiler diagnostic state after casting operations.
     */
    #define MPTAR_SUPPRESS_WARNING_CAST_QUAL_END \
        _Pragma("clang diagnostic pop")

#elif defined(__GNUC__) || defined(__GNUG__)
    #define MPTAR_SUPPRESS_WARNING_CAST_QUAL_BEGIN \
        _Pragma("GCC diagnostic push") \
        _Pragma("GCC diagnostic ignored \"-Wcast-qual\"")
    #define MPTAR_SUPPRESS_WARNING_CAST_QUAL_END \
        _Pragma("GCC diagnostic pop")
#elif defined(_MSC_VER)
    #define MPTAR_SUPPRESS_WARNING_CAST_QUAL_BEGIN \
        __pragma(warning(push)) \
        __pragma(warning(disable: 4197))
    #define MPTAR_SUPPRESS_WARNING_CAST_QUAL_END \
        __pragma(warning(pop))
#else
    #define MPTAR_SUPPRESS_WARNING_CAST_QUAL_BEGIN
    #define MPTAR_SUPPRESS_WARNING_CAST_QUAL_END
#endif
/** @} */

#endif /* MINIPAXTAR_H */
