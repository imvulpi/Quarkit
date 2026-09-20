#include "minipaxtar.h"

/**
 * \file minipaxtar.c
 * \copyright Copyright (c) 2026 Vulpi
 * \author Vulpi <78976654+imvulpi@users.noreply.github.com>
 * SPDX-License-Identifier: MIT
 */

#ifndef MPTAR_NO_STD

#include <string.h>
#include <stdlib.h>

/**
 * \brief String length utility 
 * \details Maps to standard library function: \c size_t \c strlen(const char* str).
 */
#define mptar_strlen  strlen

/**
 * \brief Bounded string length utility 
 * \details Maps to standard library function when \ref MPTAR_NO_STD is undefined: 
 * \c size_t \c strnlen(const \c char* str, size_t maxlen).
 */
#define mptar_strnlen strnlen

/**
 * \brief Bounded string copy utility 
 * \details Maps to standard library function when \ref MPTAR_NO_STD is undefined: 
 * \c char* \c strncpy(char* restrict dest, const \c char* restrict src, size_t count).
 */
#define mptar_strncpy strncpy

/**
 * \brief Memory copy utility 
 * \details Maps to standard library function when \ref MPTAR_NO_STD is undefined: 
 * \c void* \c memcpy(void* restrict dest, const \c void* restrict src, size_t size).
 */
#define mptar_memcpy  memcpy

/**
 * \brief Memory set utility 
 * \details Maps to standard library function when \ref MPTAR_NO_STD is undefined: 
 * \c void* \c memset(void* dest, int val, size_t size).
 */
#define mptar_memset  memset

/**
 * \brief Memory comparison utility 
 * \details Maps to standard library function when \ref MPTAR_NO_STD is undefined: 
 * \c int \c memcmp(const \c void* buf1, const \c void* buf2, size_t size).
 */
#define mptar_memcmp  memcmp

#else

#ifndef MPTAR_CUSTOM_STRLEN
/**
 * \brief Computes the length of a null-terminated string safely.
 * \note This function can be overridden by defining \ref MPTAR_CUSTOM_STRLEN.
 * \param str Pointer to the null-terminated string.
 * \return The number of characters preceding the terminating null byte, or 0 if \p str is \ref MPTAR_NULL.
 */
static mptar_size_t mptar_strlen(const char* str) {
    if (!str) return 0;
    mptar_size_t len = 0;
    while (str[len] != '\0') {
        len++;
    }
    return len;
}
#else
extern mptar_size_t mptar_strlen(const char* str);
#endif

#ifndef MPTAR_CUSTOM_STRNLEN
/**
 * \brief Computes the length of a string up to a maximum limit safely.
 * \note This function can be overridden by defining \ref MPTAR_CUSTOM_STRNLEN.
 * \param str Pointer to the string.
 * \param max_limit Maximum number of bytes to inspect.
 * \return The number of characters preceding the terminating null byte or \p max_limit, 
 *         whichever comes first, or 0 if \p str is \ref MPTAR_NULL.
 */
static mptar_size_t mptar_strnlen(const char* str, mptar_size_t max_limit) {
    if (!str) return 0;
    mptar_size_t count = 0;
    while (count < max_limit && str[count] != '\0') {
        count++;
    }
    return count;
}
#else
extern mptar_size_t mptar_strnlen(const char* str, mptar_size_t max_limit);
#endif

#ifndef MPTAR_CUSTOM_STRNCPY
/**
 * \brief Copies a string with a fixed maximum buffer size, padding remaining space with null bytes.
 * \note This function can be overridden by defining \ref MPTAR_CUSTOM_STRNCPY.
 * \param dst Destination buffer.
 * \param src Source null-terminated string.
 * \param n Maximum number of bytes to copy into \p dst.
 * \return Pointer to the destination buffer \p dst.
 */
static char* mptar_strncpy(char* dst, const char* src, mptar_size_t n) {
    if (dst == MPTAR_NULL || n == 0) return dst;
    
    mptar_size_t i = 0;
    
    if (src != MPTAR_NULL) {
        while (i < n && src[i] != '\0') {
            dst[i] = src[i];
            i++;
        }
    }
    
    while (i < n) {
        dst[i] = '\0';
        i++;
    }
    
    return dst;
}
#else
extern char* mptar_strncpy(char* dst, const char* src, mptar_size_t n);
#endif

#ifndef MPTAR_CUSTOM_MEMCPY
/**
 * \brief Copies a block of memory from source to destination.
 * \note This function can be overridden by defining \ref MPTAR_CUSTOM_MEMCPY.
 * \param dest Pointer to the destination buffer.
 * \param src Pointer to the source buffer.
 * \param n Number of bytes to copy.
 */
static void mptar_memcpy(void* dest, const void* src, mptar_size_t n) {
    if (!dest || !src || n == 0) return;
    
    char* d = (char*)dest;
    const char* s = (const char*)src;
    
    mptar_size_t i = 0;
    for ( ; i < n; i++) {
        d[i] = s[i];
    }
}
#else
extern void mptar_memcpy(void* dest, const void* src, mptar_size_t n);
#endif

#ifndef MPTAR_CUSTOM_MEMSET
/**
 * \brief Fills a block of memory with a specific byte value.
 * \note This function can be overridden by defining \ref MPTAR_CUSTOM_MEMSET.
 * \param data Pointer to the memory block to fill.
 * \param value Byte value to set.
 * \param amount Number of bytes to set.
 */
static void mptar_memset(void* data, char value, mptar_size_t amount) {
    if (!data || amount == 0) return;
    
    char* bytes = (char*)data;
    mptar_size_t i = 0;
    for ( ; i < amount; i++) {
        bytes[i] = value;
    }
}
#else
extern void mptar_memset(void* data, char value, mptar_size_t amount);
#endif

#ifndef MPTAR_CUSTOM_MEMCMP
/**
 * \brief Compares two blocks of memory.
 * \note This function can be overridden by defining \ref MPTAR_CUSTOM_MEMCMP.
 * \param s1 Pointer to the first memory block.
 * \param s2 Pointer to the second memory block.
 * \param size Number of bytes to compare.
 * \return 0 if the blocks match, a negative value if \p s1 is less than \p s2, or a positive value if \p s1 is greater than \p s2.
 */
static int mptar_memcmp(const void* s1, const void* s2, mptar_size_t size) {
    if (!s1 || !s2 || size == 0) return 0;
    
    const unsigned char* p1 = (const unsigned char*)s1;
    const unsigned char* p2 = (const unsigned char*)s2;

    mptar_size_t i = 0; 
    for (; i < size; i++) {
        if (p1[i] != p2[i]) {
            return (p1[i] < p2[i]) ? -1 : 1;
        }
    }

    return 0;
}
#else
extern int mptar_memcmp(const void* s1, const void* s2, mptar_size_t size);
#endif

#endif /* MPTAR_NO_STD */

#ifndef MPTAR_NO_WRITE

#if defined(MPTAR_CUSTOM_IU64TOA)
    #define MPTAR_CUSTOM_U64TOA
    #define MPTAR_CUSTOM_I64TOA
#endif

#ifndef MPTAR_CUSTOM_U64TOA
/**
 * \brief Converts an unsigned 64-bit integer into an octal string representation.
 * \details Formats the numeric value as an octal string and guarantees a null-terminated 
 *          output within the specified buffer size.
 * \note This function can be overridden by defining \ref MPTAR_CUSTOM_U64TOA.
 * \param value The 64-bit unsigned integer to convert.
 * \param str Output buffer to store the resulting null-terminated octal string.
 * \param str_size Maximum capacity of the output buffer in bytes.
 * \param[out] out_err Output status pointer receiving an error code on failure (e.g., overflow if the buffer was too small).
 * \return Pointer to the resulting string buffer \p str on success, or \ref MPTAR_NULL on failure.
 */
static char* mptar_u64toa(mptar_uint64 value, char* str, mptar_size_t str_size, int *out_err){
    if (out_err) *out_err = MPTAR_OK;
    if (str == MPTAR_NULL || str_size < 2) {
        if (out_err) *out_err = MPTAR_ERR_INVALID_ARG;
        return MPTAR_NULL;
    }

    mptar_size_t i = 0;
    if (value == 0) {
        str[0] = '0';
        str[1] = '\0';
        return str;
    }

    while (value > 0 && i < (str_size - 1)) {
        str[i++] = (char)((value % 10) + '0');
        value /= 10;
    }

    if (value > 0) {
        if (out_err) *out_err = MPTAR_ERR_OVERFLOW;
        str[str_size - 1] = '\0';
        return str;
    }

    str[i] = '\0';

    mptar_size_t start = 0;
    mptar_size_t end = i - 1;
    while (start < end) {
        char temp = str[start];
        str[start] = str[end];
        str[end] = temp;
        start++;
        end--;
    }

    return str;
}

#else

extern char* mptar_u64toa(mptar_uint64 value, char* str, mptar_size_t str_size, int *out_err);

#endif

#ifndef MPTAR_CUSTOM_I64TOA
/**
 * \brief Converts a signed 64-bit integer into a string representation.
 * \details Formats the numeric value as a decimal (or signed) string, including a negative sign 
 *          when applicable, and guarantees a null-terminated output within the specified buffer size.
 * \note This function can be overridden by defining \ref MPTAR_CUSTOM_I64TOA.
 * \param value The 64-bit signed integer to convert.
 * \param buf Output buffer to store the resulting null-terminated string.
 * \param buf_size Maximum capacity of the output buffer in bytes.
 * \param[out] out_err Output status pointer receiving an error code on failure (e.g., \ref MPTAR_ERR_INVALID_ARG).
 * \return Pointer to the resulting string buffer \p buf on success, or \ref MPTAR_NULL on failure.
 */
static char* mptar_i64toa(mptar_int64 value, char* buf, mptar_size_t buf_size, int *out_err) {
    if (out_err) *out_err = MPTAR_OK;
    
    if (buf == MPTAR_NULL || buf_size <= 1) {
        if (out_err) *out_err = MPTAR_ERR_INVALID_ARG;
        return MPTAR_NULL;
    }

    if (value < 0) {
        mptar_uint64 abs_val;
        if (value == MPTAR_INT64_MIN) {
            abs_val = (mptar_uint64)MPTAR_INT64_MAX + 1ULL;
        } else {
            abs_val = (mptar_uint64)(-value);
        }
        
        char* result = mptar_u64toa(abs_val, buf + 1, buf_size - 1, out_err);
        if (result == MPTAR_NULL) {
            return MPTAR_NULL;
        }

        buf[0] = '-';
        return buf;
    } else {
        return mptar_u64toa((mptar_uint64)value, buf, buf_size, out_err);
    }
}

#else

extern char* mptar_i64toa(mptar_int64 value, char* buf, mptar_size_t buf_size, int *out_err);

#endif

#ifndef MPTAR_CUSTOM_STREAM_WRITE_ZEROES
/**
 * \brief Writes a specified number of zero bytes to the underlying stream.
 * \details Iteratively writes zero-filled blocks to the output stream via the writer's 
 *          configured callback until the required amount is fully satisfied.
 * \note This function can be overridden by defining \c MPTAR_CUSTOM_STREAM_WRITE_ZEROES.
 * \warning Overriding this function carries no stability guarantees; 
 *          it may be modified or removed entirely in future versions 
 *          without notice or deprecation.
 * \param ctx Pointer to initialized writer context containing the I/O callback.
 * \param amount Number of zero-bytes required to write to the stream.
 * \return \ref MPTAR_OK on success, or a negative error code (e.g., \ref MPTAR_ERR_INVALID_ARG or \ref MPTAR_ERR_IO_WRITE).
 */
static int mptar_stream_write_zeroes(mptar_writer* ctx, mptar_size_t amount) {
    if (!ctx) return MPTAR_ERR_INVALID_ARG;
    if (amount == 0) return MPTAR_OK;

    const char zero_chunk[16] = {0}; 
    mptar_size_t total_written = 0;

    while (total_written < amount) {
        mptar_size_t remaining = amount - total_written;
        mptar_size_t chunk_size = (remaining > sizeof(zero_chunk)) ? sizeof(zero_chunk) : remaining;

        mptar_size_t res = ctx->write(ctx->write_user_data, zero_chunk, chunk_size);
        if (res == 0 || res > chunk_size) return MPTAR_ERR_IO_WRITE;
        total_written += chunk_size;
    }

    return MPTAR_OK;
}
#else
extern int mptar_stream_write_zeroes(mptar_writer* ctx, mptar_size_t padding_needed);
#endif

#ifndef MPTAR_CUSTOM_PAX_CALCULATE_RECORD_LEN
/**
 * \brief Calculates the exact encoded length of a PAX extended header record including its length prefix.
 * \details PAX records use a variable-length format where the record length prefix includes itself 
 *          in the byte count. This function computes the final length dynamically, accounting for 
 *          digit length boundary shifts.
 * \note This function can be overridden by defining \c MPTAR_CUSTOM_PAX_CALCULATE_RECORD_LEN.
 * \warning Overriding this function carries no stability guarantees; 
 *          it may be modified or removed entirely in future versions 
 *          without notice or deprecation.
 * \param data_len Length of the PAX record data payload (excluding the length prefix itself).
 * \return The total length of the PAX record in bytes, or 0 if \p data_len exceeds \ref MPTAR_PAX_MAX_DATA_LEN.
 */
static mptar_uint32 mptar_pax_calculate_record_len(mptar_uint32 data_len) {
    if (data_len > MPTAR_PAX_MAX_DATA_LEN) return 0; /* Would overflow */

    mptar_uint32 digits = 1;
    if      (data_len >= 1000000000U) digits = 10;
    else if (data_len >= 100000000U)  digits = 9;
    else if (data_len >= 10000000U)   digits = 8;
    else if (data_len >= 1000000U)    digits = 7;
    else if (data_len >= 100000U)     digits = 6;
    else if (data_len >= 10000U)      digits = 5;
    else if (data_len >= 1000U)       digits = 4;
    else if (data_len >= 100U)        digits = 3;
    else if (data_len >= 10U)         digits = 2;

    mptar_uint32 total = data_len + digits;
    
    mptar_uint32 final_digits = 1;
    if      (total >= 1000000000U) final_digits = 10;
    else if (total >= 100000000U)  final_digits = 9;
    else if (total >= 10000000U)   final_digits = 8;
    else if (total >= 1000000U)    final_digits = 7;
    else if (total >= 100000U)     final_digits = 6;
    else if (total >= 10000U)      final_digits = 5;
    else if (total >= 1000U)       final_digits = 4;
    else if (total >= 100U)        final_digits = 3;
    else if (total >= 10U)         final_digits = 2;

    return data_len + final_digits;
}
#else
extern mptar_uint32 mptar_pax_calculate_record_len(mptar_uint32 data_len);
#endif

#ifndef MPTAR_CUSTOM_PAX_FORMAT_TIME
/**
 * \brief Formats a timestamp with fractional nanoseconds into a PAX-compatible string representation.
 * \details Formats a seconds integer combined with optional nanoseconds into a standard fractional 
 *          seconds string format (e.g., seconds.nanoseconds), handling padding and trailing zero truncation.
 * \note This function can be overridden by defining \c MPTAR_CUSTOM_PAX_FORMAT_TIME.
 * \warning Overriding this function carries no stability guarantees; 
 *          it may be modified or removed entirely in future versions 
 *          without notice or deprecation.
 * \param sec The signed 64-bit seconds component of the timestamp.
 * \param nsec The unsigned 32-bit nanoseconds component of the timestamp.
 * \param str Output buffer to store the resulting null-terminated timestamp string.
 * \param size Maximum capacity of the output buffer in bytes.
 * \param[out] out_err Output status pointer receiving an error code on failure (e.g., \ref MPTAR_ERR_INVALID_ARG or \ref MPTAR_ERR_BUFFER_TOO_SMALL).
 * \return Pointer to the resulting string buffer \p str on success, or \ref MPTAR_NULL on failure.
 */
static char* mptar_pax_format_time(mptar_int64 sec, mptar_uint32 nsec, char* str, mptar_size_t size, int *out_err){
    if(out_err) *out_err = MPTAR_OK;
    if(str == MPTAR_NULL || size == 0){
        if(out_err) *out_err = MPTAR_ERR_INVALID_ARG;
        return MPTAR_NULL;
    }

    int error = MPTAR_OK;
    mptar_i64toa(sec, str, size, &error);
    if (error != MPTAR_OK) {
        if(out_err) *out_err = error;
        return MPTAR_NULL;
    }

    if (nsec == 0) {
        return str;
    }

    mptar_size_t int_size = mptar_strlen(str);

    char nsec_buf[10] = {0}; /* Space for 9 digits + \0 */
    int nsec_err = MPTAR_OK;
    mptar_u64toa(nsec, nsec_buf, sizeof(nsec_buf), &nsec_err);
    if (nsec_err != MPTAR_OK) {
        if(out_err) *out_err = MPTAR_ERR_NS_CONVERSION_FAILED;
        return str; 
    }

    mptar_size_t nsec_len = mptar_strlen(nsec_buf);
    char formatted_nsec[sizeof(nsec_buf)];
    mptar_size_t leading_zeros = sizeof(formatted_nsec) - 1 - nsec_len;

    if (leading_zeros > 0) mptar_memset(formatted_nsec, '0', leading_zeros);
    if (nsec_len > 0) mptar_memcpy(&formatted_nsec[leading_zeros], nsec_buf, nsec_len);
    formatted_nsec[sizeof(formatted_nsec) - 1] = '\0';

    int end = sizeof(formatted_nsec) - 2;
    while (end > 0 && formatted_nsec[end] == '0') {
        formatted_nsec[end] = '\0';
        end--;
    }

    mptar_size_t final_nsec_len = mptar_strlen(formatted_nsec);
    mptar_size_t required_bytes = int_size + final_nsec_len + MPTAR_PAX_NSEC_OVERHEAD;
    if (required_bytes > size) {
        if(out_err) *out_err = MPTAR_ERR_BUFFER_TOO_SMALL;
        return str;
    }

    str[int_size] = '.';
    mptar_memcpy(&str[int_size + 1], formatted_nsec, final_nsec_len);
    str[int_size + 1 + final_nsec_len] = '\0';

    return str;
}
#else
extern char* mptar_pax_format_time(mptar_int64 sec, mptar_uint32 nsec, char* str, mptar_size_t size, int *out_err);
#endif

#ifndef MPTAR_CUSTOM_PAX_STREAM_RECORD
/**
 * \brief Streams a single PAX extended header record to the underlying output storage.
 * \details Formats and writes the variable-length PAX record components in sequence 
 *          (`[size] [space] [keyword] [=] [value] [\n]`) via the writer's I/O stream callback.
 * \note This function can be overridden by defining \c MPTAR_CUSTOM_PAX_STREAM_RECORD.
 * \warning Overriding this function carries no stability guarantees; 
 *          it may be modified or removed entirely in future versions 
 *          without notice or deprecation.
 * \param ctx Pointer to initialized writer context containing the I/O callback and user data.
 * \param record_size Total pre-calculated byte length of the PAX record.
 * \param keyword Null-terminated PAX attribute keyword string (e.g., \c path or \c size ).
 * \param value Pointer to the attribute value buffer.
 * \param value_len Number of bytes in the attribute value buffer.
 * \return \ref MPTAR_OK on success, or \ref MPTAR_ERR_IO_WRITE if any write operation fails or sends an unexpected byte count.
 */
static int mptar_pax_stream_record(mptar_writer* ctx, mptar_uint32 record_size, const char* keyword, const char* value, mptar_size_t value_len) {
    char size_str[12];
    mptar_u64toa(record_size, size_str, sizeof(size_str), MPTAR_NULL);
    mptar_size_t size_len = mptar_strlen(size_str);
    mptar_size_t kw_len = mptar_strlen(keyword);

    if (ctx->write(ctx->write_user_data, size_str, size_len) != size_len) return MPTAR_ERR_IO_WRITE;
    if (ctx->write(ctx->write_user_data, " ", 1) != 1) return MPTAR_ERR_IO_WRITE;
    if (ctx->write(ctx->write_user_data, keyword, kw_len) != kw_len) return MPTAR_ERR_IO_WRITE;
    if (ctx->write(ctx->write_user_data, "=", 1) != 1) return MPTAR_ERR_IO_WRITE;
    if (ctx->write(ctx->write_user_data, value, value_len) != value_len) return MPTAR_ERR_IO_WRITE;
    if (ctx->write(ctx->write_user_data, "\n", 1) != 1) return MPTAR_ERR_IO_WRITE;

    return MPTAR_OK;
}
#else
extern int mptar_pax_stream_record(mptar_writer* ctx, mptar_uint32 record_size, const char* keyword, const char* value, mptar_size_t value_len);
#endif

#ifndef MPTAR_CUSTOM_WRITE_OCTAL_FIELD
/**
 * \brief Formats and serializes an integer value into a USTAR/tar header field as an octal string or binary representation.
 * \details Safely converts a 64-bit integer into its standard octal string format (including null termination or padding). 
 *          If the value exceeds the maximum capacity of the available octal character width, it automatically falls back 
 *          to the standard tar binary encoding extension (setting the high-bit flag and a binary value in big endian).
 * \note This function can be overridden by defining \c MPTAR_CUSTOM_WRITE_OCTAL_FIELD.
 * \warning Overriding this function carries no stability guarantees; 
 *          it may be modified or removed entirely in future versions 
 *          without notice or deprecation.
 * \param dst Pointer to the destination header field buffer.
 * \param value The 64-bit unsigned integer value to write.
 * \param size Total size capacity of the destination field in bytes.
 */
static void mptar_write_octal_field(char* dst, mptar_uint64 value, mptar_size_t size)
{
    if(dst == MPTAR_NULL || size == 0) return;

    mptar_size_t octal_digits = size - 1; 
    mptar_uint64 max_octal_val = 0;
    
    if (octal_digits >= MPTAR_MAX_OCTAL_DIGITS_64) {
        max_octal_val = MPTAR_UINT64_MAX;
    } else {
        max_octal_val = ((mptar_uint64)1 << (octal_digits * 3)) - 1;
    }

    if (value > max_octal_val) {
        mptar_size_t i = size;

        while(i > 0){
            dst[--i] = (char)(value & 0xFF);
            value >>= 8;
        }

        dst[0] |= MPTAR_OCTAL_BINARY_FLAG;
        return;
    }

    mptar_memset(dst, '0', size);
    dst[size - 1] = '\0';

    mptar_size_t i = octal_digits;
    while (i > 0) {
        dst[--i] = '0' + (value & 7);
        value >>= 3;
    }

    dst[octal_digits] = '\0';
}
#else
extern void mptar_write_octal_field(char* dst, mptar_uint64 value, mptar_size_t size);
#endif

#ifndef MPTAR_CUSTOM_CALCULATE_HEADER_CHECKSUM
/**
 * \brief Calculates the standard POSIX USTAR header checksum value.
 * \details Computes the sum of all bytes in the 512-byte tar header block, treating the checksum field 
 *          itself as if it were filled with space characters (ASCII 32) according to the USTAR standard specification.
 * \note This function can be overridden by defining \c MPTAR_CUSTOM_CALCULATE_HEADER_CHECKSUM.
 * \warning Overriding this function carries no stability guarantees; 
 *          it may be modified or removed entirely in future versions 
 *          without notice or deprecation.
 * \param header_block Pointer to the 512-byte USTAR header block buffer.
 * \return The calculated unsigned 32-bit checksum value, or 0 if \p header_block is \ref MPTAR_NULL.
 */
static mptar_uint32 mptar_calculate_header_checksum(char* header_block){
    if (header_block == MPTAR_NULL) return 0;

    const unsigned char* block = (const unsigned char*)header_block;
    mptar_uint32 sum = 0;    

    mptar_uint16 i = 0;
    for ( ; i < MPTAR_CHECKSUM_OFFSET; i++) {
        sum += block[i];
    }

    /* Hardcoded constant value for the 8 space characters (8 * 32) */
    sum += MPTAR_SPACE_CHECKSUM_VAL; 

    for (i = MPTAR_CHECKSUM_END_OFFSET; i < MPTAR_USTAR_HEADER_SIZE; i++) {
        sum += block[i];
    }
    
    return sum;
}
#else
extern mptar_uint32 mptar_calculate_header_checksum(char* header_block);
#endif

#ifndef MPTAR_CUSTOM_WRITE_HEADER_CHECKSUM
/**
 * \brief Computes and writes the standard POSIX USTAR header checksum into its designated block field.
 * \details Calculates the checksum using \ref mptar_calculate_header_checksum, serializes it into 
 *          an octal field format via \ref mptar_write_octal_field, and applies the trailing space/null terminator 
 *          padding mandated by the USTAR header specification.
 * \note This function can be overridden by defining \c MPTAR_CUSTOM_WRITE_HEADER_CHECKSUM.
 * \warning Overriding this function carries no stability guarantees; 
 *          it may be modified or removed entirely in future versions 
 *          without notice or deprecation.
 * \param header_block Pointer to the 512-byte USTAR header block buffer.
 */
static void mptar_write_header_checksum(char* header_block) {
    if (header_block == MPTAR_NULL) return;
    mptar_uint32 sum = mptar_calculate_header_checksum(header_block);
    mptar_write_octal_field(header_block + MPTAR_CHECKSUM_OFFSET, sum, 7);
    header_block[MPTAR_CHECKSUM_END_OFFSET - 1] = ' ';
}
#else
extern void mptar_write_header_checksum(char* header_block);
#endif

#ifndef MPTAR_CUSTOM_CAN_PATH_FIT_USTAR
/**
 * \brief Checks whether a given file path can fit into the standard POSIX USTAR header path and prefix fields.
 * \details Validates if the total path length fits within the standard name field, or if it can be 
 *          successfully split at a directory separator (`/`) into the USTAR prefix and name fields without 
 *          violating field length restrictions.
 * \note This function can be overridden by defining \c MPTAR_CUSTOM_CAN_PATH_FIT_USTAR.
 * \warning Overriding this function carries no stability guarantees; 
 *          it may be modified or removed entirely in future versions 
 *          without notice or deprecation.
 * \param path Pointer to the character string representing the file path.
 * \param len Number of bytes/characters in the path string.
 * \return \c true if the path fits into standard USTAR fields, or \c false if it is too long or requires a PAX extended header.
 */
static bool mptar_can_path_fit_ustar(const char* path, mptar_size_t len) {
    if (!path) return false;

    if (len <= MPTAR_USTAR_SIZE_NAME) {
        return true;
    }else if (len > (MPTAR_USTAR_SIZE_PREFIX + 1 + MPTAR_USTAR_SIZE_NAME)) {
        return false; 
    }

    mptar_size_t i = len - 1;
    for ( ; i > 0; i--) {
        mptar_size_t name_len = len - i - 1;
        
        if (name_len > MPTAR_USTAR_SIZE_NAME) {
            break; 
        }

        if (path[i] == '/') {
            mptar_size_t prefix_len = i;
            
            if (prefix_len <= MPTAR_USTAR_SIZE_PREFIX && name_len <= MPTAR_USTAR_SIZE_NAME) {
                return true; 
            }
        }
    }

    return false;
}
#else
extern bool mptar_can_path_fit_ustar(const char* path, mptar_size_t len);
#endif

#ifndef MPTAR_CUSTOM_WRITE_USTAR_PATH
/**
 * \brief Writes and splits a file path into the standard POSIX USTAR header name and prefix fields.
 * \details Serializes a full file path into the USTAR header fields. If the path fits within the 
 *          standard name limits, it is written directly. If it exceeds standard name size but can be split 
 *          at a directory separator, it populates both the prefix and name fields. If it cannot fit within 
 *          USTAR boundaries at all, it truncates the name and signals that a PAX extended header is required.
 * \note This function can be overridden by defining \c MPTAR_CUSTOM_WRITE_USTAR_PATH.
 * \warning Overriding this function carries no stability guarantees; 
 *          it may be modified or removed entirely in future versions 
 *          without notice or deprecation.
 * \param header_name Pointer to the destination USTAR header name field buffer.
 * \param header_prefix Pointer to the destination USTAR header prefix field buffer (optional, may be \ref MPTAR_NULL).
 * \param full_path Pointer to the null-terminated full file path string to encode.
 * \return \ref MPTAR_OK on success, \ref MPTAR_NEEDS_PAX if the path requires PAX extension headers, or an error code (e.g., \ref MPTAR_ERR_INVALID_ARG).
 */
static int mptar_write_ustar_path(char* header_name, char* header_prefix, const char* full_path)
{
    if(header_name == MPTAR_NULL || full_path == MPTAR_NULL) return MPTAR_ERR_INVALID_ARG; /* header_prefix can be null. */

    mptar_size_t path_len = mptar_strlen(full_path);
    if (path_len <= MPTAR_USTAR_SIZE_NAME) {
        mptar_memcpy(header_name, full_path, path_len);
        if (path_len < MPTAR_USTAR_SIZE_NAME) {
            header_name[path_len] = '\0';
        }
        if (header_prefix != MPTAR_NULL) {
            header_prefix[0] = '\0';
        }
        return MPTAR_OK;
    }

    if (header_prefix == MPTAR_NULL) {
        mptar_memcpy(header_name, full_path, MPTAR_USTAR_SIZE_NAME);
        return MPTAR_NEEDS_PAX;
    }

    if (path_len > (MPTAR_USTAR_SIZE_PREFIX + 1 + MPTAR_USTAR_SIZE_NAME)) {
        mptar_memcpy(header_name, full_path, MPTAR_USTAR_SIZE_NAME);
        return MPTAR_NEEDS_PAX;
    }

    int split_index = -1;
    mptar_size_t i = 0;
    for ( ; i < path_len; i++) {
        if (full_path[i] == '/') {
            mptar_size_t prefix_part_len = i;
            mptar_size_t name_part_len = path_len - i - 1;

            if (prefix_part_len <= MPTAR_USTAR_SIZE_PREFIX && name_part_len <= MPTAR_USTAR_SIZE_NAME) {
                if (name_part_len == 0 && split_index != -1) {
                    continue; 
                }
                split_index = (int)i;
            }
        }
    }

    if (split_index == -1) {
        mptar_memcpy(header_name, full_path, MPTAR_USTAR_SIZE_NAME);
        return MPTAR_NEEDS_PAX;
    }

    mptar_size_t final_prefix_len = (mptar_size_t)split_index;
    mptar_size_t final_name_len = path_len - final_prefix_len - 1;

    mptar_memcpy(header_prefix, full_path, final_prefix_len);
    if (final_prefix_len < MPTAR_USTAR_SIZE_PREFIX) {
        header_prefix[final_prefix_len] = '\0';
    }

    mptar_memcpy(header_name, full_path + final_prefix_len + 1, final_name_len);
    if (final_name_len < MPTAR_USTAR_SIZE_NAME) {
        header_name[final_name_len] = '\0';
    }

    return MPTAR_OK;
}
#else
extern int mptar_write_ustar_path(char* header_name, char* header_prefix, const char* full_path);
#endif

#ifndef MPTAR_CUSTOM_WRITE_USTAR_HEADER
/**
 * \brief Populates and serializes a standard POSIX USTAR header block from file metadata.
 * \details Clears the header buffer, encodes the file path, link targets, ownership credentials, 
 *          permissions, major/minor device numbers, size, and modification timestamp into their 
 *          respective USTAR field formats. If any field exceeds standard USTAR boundaries (such as large file sizes 
 *          or long paths), it automatically marks the status to indicate that a PAX extended header is required.
 * \note This function can be overridden by defining \c MPTAR_CUSTOM_WRITE_USTAR_HEADER.
 * \warning Overriding this function carries no stability guarantees; 
 *          it may be modified or removed entirely in future versions 
 *          without notice or deprecation.
 * \param header Pointer to the destination 512-byte \ref mptar_header structure buffer to populate.
 * \param meta Pointer to the source \ref mptar_metadata structure containing the archive entry's file metadata.
 * \return \ref MPTAR_OK on success, \ref MPTAR_NEEDS_PAX if the path, linkpath or size metadata require PAX extension headers, or an error code (e.g., \ref MPTAR_ERR_INVALID_ARG).
 */
static int mptar_write_ustar_header(mptar_header* header, const mptar_metadata* meta){
    if (header == MPTAR_NULL || meta == MPTAR_NULL) {
        return MPTAR_ERR_INVALID_ARG;
    }
    if (meta->path == MPTAR_NULL) {
        return MPTAR_ERR_INVALID_ARG;
    }

    mptar_memset(header, 0, MPTAR_USTAR_HEADER_SIZE);
    
    int result_status = MPTAR_OK;

    if (meta->typeflag == '1' || meta->typeflag == '2') {
        if (meta->link_target != MPTAR_NULL) {
            int link_res = mptar_write_ustar_path(header->linkname, MPTAR_NULL, meta->link_target);
            if (link_res == MPTAR_NEEDS_PAX) {
                result_status = MPTAR_NEEDS_PAX;
            }
        }
    }

    int path_res = mptar_write_ustar_path(header->name, header->prefix, meta->path);
    if (path_res == MPTAR_NEEDS_PAX) {
        result_status = MPTAR_NEEDS_PAX;
    }
    
    if (meta->uname != MPTAR_NULL) {
        mptar_strncpy(header->uname, meta->uname, sizeof(header->uname));
        header->uname[(sizeof(header->uname) - 1)] = '\0';
    }

    if (meta->gname != MPTAR_NULL) {
        mptar_strncpy(header->gname, meta->gname, sizeof(header->gname));
        header->gname[(sizeof(header->gname) - 1)] = '\0';
    }

    mptar_write_octal_field(header->gid, meta->gid, sizeof(header->gid));
    mptar_write_octal_field(header->uid, meta->uid, sizeof(header->uid));
    mptar_write_octal_field(header->mode, (mptar_uint64)meta->mode, sizeof(header->mode));

#ifdef MPTAR_SUPPORT_SPECIAL
    if (meta->typeflag == '3' || meta->typeflag == '4') {
        mptar_write_octal_field(header->devmajor, meta->devmajor, sizeof(header->devmajor));
        mptar_write_octal_field(header->devminor, meta->devminor, sizeof(header->devminor));
    }
#endif
    
    mptar_uint64 size_to_write = meta->size;
    if (size_to_write >= MPTAR_USTAR_MAX_OCTAL_12B) {
        size_to_write = 0;
        result_status = MPTAR_NEEDS_PAX;
    }
    mptar_write_octal_field(header->size, size_to_write, sizeof(header->size));

    if(meta->mtime.has_value){
        mptar_write_octal_field(header->mtime, (mptar_uint64)meta->mtime.value.sec, sizeof(header->mtime));
    }

    header->typeflag = meta->typeflag;
    mptar_memcpy(header->magic, "ustar\0", sizeof(header->magic));
    mptar_memcpy(header->version, "00", sizeof(header->version));
    mptar_write_header_checksum((char*)header);

    return result_status;
}
#else
extern int mptar_write_ustar_header(mptar_header* header, const mptar_metadata* meta);
#endif

#ifndef MPTAR_CUSTOM_WRITE_PAX_HEADER
/**
 * \brief Constructs and streams a POSIX PAX extended header block (`typeflag 'x'`) to the output archive.
 * \details Allocates a temporary USTAR header block, populates it with PAX metadata attributes (including 
 *          a standard fallback path such as `PaxHeader/dump` and the exact payload size), writes it to the 
 *          underlying stream via the writer's I/O callback, and cleans up the allocated memory buffer.
 * \note This function can be overridden by defining \c MPTAR_CUSTOM_WRITE_PAX_HEADER.
 * \warning Overriding this function carries no stability guarantees; 
 *          it may be modified or removed entirely in future versions 
 *          without notice or deprecation.
 * \param ctx Pointer to initialized writer context containing memory allocators and I/O callbacks.
 * \param size Exact total size in bytes of the following PAX payload data records.
 * \param meta Pointer to the source \ref mptar_metadata structure containing the entry's original attributes.
 * \return \ref MPTAR_OK on success, or a negative error code (e.g., \ref MPTAR_ERR_INVALID_ARG, \ref MPTAR_ERR_ALLOC, or \ref MPTAR_ERR_IO_WRITE).
 */
static int mptar_write_pax_header(mptar_writer* ctx, mptar_uint64 size, const mptar_metadata* meta){
    if (ctx == MPTAR_NULL || meta == MPTAR_NULL) {
        return MPTAR_ERR_INVALID_ARG;
    }

    char* header_bytes = (char*)ctx->memory.alloc(ctx->memory.alloc_user_data, MPTAR_USTAR_HEADER_SIZE);
    if(header_bytes == MPTAR_NULL){
        return MPTAR_ERR_ALLOC;
    }

    mptar_header* header = (mptar_header*)header_bytes;
    mptar_metadata temp_meta = {0};

    temp_meta.path = "PaxHeader/dump";
    temp_meta.size = size; /* Size of pax payload */
    temp_meta.mode = meta->mode;
    temp_meta.uid = meta->uid;
    temp_meta.gid = meta->gid;
    temp_meta.mtime = meta->mtime;
    temp_meta.typeflag = 'x';

    mptar_write_ustar_header(header, &temp_meta);
    
    mptar_size_t bytes_written = ctx->write(ctx->write_user_data, header_bytes, MPTAR_USTAR_HEADER_SIZE);
    ctx->memory.free(ctx->memory.alloc_user_data, header_bytes);

    if (bytes_written != MPTAR_USTAR_HEADER_SIZE) {
        return MPTAR_ERR_IO_WRITE;
    }

    return MPTAR_OK;
}
#else
extern int mptar_write_pax_header(mptar_writer* ctx, mptar_uint64 size, const mptar_metadata* meta);
#endif

int mptar_write_header(mptar_writer* ctx, const mptar_metadata* meta){
    if (ctx == MPTAR_NULL || meta == MPTAR_NULL || meta->path == MPTAR_NULL || 
        ctx->memory.alloc == MPTAR_NULL || ctx->memory.free == MPTAR_NULL || ctx->write == MPTAR_NULL) {
        return MPTAR_ERR_INVALID_ARG;
    }

    if(ctx->bytes_left > 0) return MPTAR_ERR_INCOMPLETE_PAYLOAD;

#ifndef MPTAR_SUPPORT_SPECIAL
    if (meta->typeflag == '3' || meta->typeflag == '4') {
        return MPTAR_ERR_UNSUPPORTED_TYPE;
    }
#endif
    
    mptar_size_t path_len = mptar_strlen(meta->path);
    mptar_size_t link_target_size = (meta->link_target == MPTAR_NULL) ? 0 : mptar_strlen(meta->link_target);
    mptar_size_t uname_len = (meta->uname == MPTAR_NULL) ? 0 : mptar_strlen(meta->uname);
    mptar_size_t gname_len = (meta->gname == MPTAR_NULL) ? 0 : mptar_strlen(meta->gname);

    bool large_path = !mptar_can_path_fit_ustar(meta->path, path_len);
    bool large_size = ((ctx->flags & MPTAR_CTX_ALLOW_PAX_FOR_OCTAL) != 0) && (meta->size >= MPTAR_USTAR_MAX_OCTAL_12B);
    bool large_link_target = (link_target_size > MPTAR_USTAR_MAX_LEN_LINKNAME);
    bool uid_needs_pax = (meta->uid > MPTAR_USTAR_MAX_OCTAL_8B) && (((ctx->flags & MPTAR_CTX_ALLOW_PAX_FOR_OCTAL) != 0) || meta->uid > MPTAR_BINARY_MAX_8B);
    bool gid_needs_pax = (meta->gid > MPTAR_USTAR_MAX_OCTAL_8B) && (((ctx->flags & MPTAR_CTX_ALLOW_PAX_FOR_OCTAL) != 0) || meta->gid > MPTAR_BINARY_MAX_8B);
    bool uname_needs_pax = (uname_len > MPTAR_USTAR_MAX_LEN_UNAME);
    bool gname_needs_pax = (gname_len > MPTAR_USTAR_MAX_LEN_GNAME);

    bool mtime_needs_pax = meta->mtime.has_value && 
        (meta->mtime.value.sec < 0 || meta->mtime.value.nsec > 0
        || (meta->mtime.value.sec > (mptar_int64)MPTAR_USTAR_MAX_OCTAL_12B && ((ctx->flags & MPTAR_CTX_ALLOW_PAX_FOR_OCTAL) != 0)));
    
    #ifdef MPTAR_SUPPORT_EXTRA_TIMES
        bool times_need_pax = mtime_needs_pax || meta->atime.has_value || meta->ctime.has_value;
    #else
        bool times_need_pax = mtime_needs_pax;
    #endif

    int res = MPTAR_OK;
    bool need_pax = large_path || large_size || large_link_target || uid_needs_pax || gid_needs_pax || uname_needs_pax || gname_needs_pax || times_need_pax;
    if(need_pax) {
        mptar_uint64 pax_header_size = 0;
        char size_str_buf[MPTAR_UINT64_STR_BUF_SIZE] = {0};
        mptar_size_t size_str_len = 0;
        
        mptar_uint32 pax_path_size = 0;
        mptar_uint32 pax_size_size = 0;
        mptar_uint32 pax_link_target_size = 0;
        mptar_uint32 pax_mtime_size = 0;
        char mtime_str_buf[MPTAR_TIMESPEC_STR_BUF_SIZE];
        mptar_size_t mtime_str_len = 0;
        char uid_str_buf[MPTAR_UINT64_STR_BUF_SIZE];
        char gid_str_buf[MPTAR_UINT64_STR_BUF_SIZE];
        mptar_size_t uid_str_len = 0;
        mptar_size_t gid_str_len = 0;

        mptar_uint32 pax_uid_size = 0;
        mptar_uint32 pax_gid_size = 0;
        mptar_uint32 pax_uname_size = 0;
        mptar_uint32 pax_gname_size = 0;

#ifdef MPTAR_SUPPORT_EXTRA_TIMES
        mptar_uint32 pax_atime_size = 0;
        mptar_uint32 pax_ctime_size = 0;
        char atime_str_buf[MPTAR_TIMESPEC_STR_BUF_SIZE];
        char ctime_str_buf[MPTAR_TIMESPEC_STR_BUF_SIZE];
        mptar_size_t atime_str_len = 0;
        mptar_size_t ctime_str_len = 0;
#endif

        if (large_path) {
            pax_path_size = mptar_pax_calculate_record_len((mptar_uint32)path_len + MPTAR_PAX_OVERHEAD_PATH);
            pax_header_size += pax_path_size;
        }

        if (large_size) {
            mptar_u64toa(meta->size, size_str_buf, sizeof(size_str_buf), &res);
            if (res != MPTAR_OK) return res;
            size_str_len = mptar_strlen(size_str_buf);
            pax_size_size = mptar_pax_calculate_record_len((mptar_uint32)size_str_len + MPTAR_PAX_OVERHEAD_SIZE);
            pax_header_size += pax_size_size;
        }
        
        if (large_link_target) {
            pax_link_target_size = mptar_pax_calculate_record_len((mptar_uint32)link_target_size + MPTAR_PAX_OVERHEAD_LINK);
            pax_header_size += pax_link_target_size;
        }

        if (mtime_needs_pax) {
            mptar_pax_format_time(meta->mtime.value.sec, meta->mtime.value.nsec, mtime_str_buf, sizeof(mtime_str_buf), &res);
            if (res != MPTAR_OK) return res;
            mtime_str_len = mptar_strlen(mtime_str_buf);
            pax_mtime_size = mptar_pax_calculate_record_len((mptar_uint32)mtime_str_len + MPTAR_PAX_OVERHEAD_TIME);
            pax_header_size += pax_mtime_size;
        }

#ifdef MPTAR_SUPPORT_EXTRA_TIMES
        if (meta->atime.has_value) {
            mptar_pax_format_time(meta->atime.value.sec, meta->atime.value.nsec, atime_str_buf, sizeof(atime_str_buf), &res);
            if (res != MPTAR_OK) return res;
            atime_str_len = mptar_strlen(atime_str_buf);
            pax_atime_size = mptar_pax_calculate_record_len((mptar_uint32)atime_str_len + MPTAR_PAX_OVERHEAD_TIME);
            pax_header_size += pax_atime_size;
        }

        if (meta->ctime.has_value) {
            mptar_pax_format_time(meta->ctime.value.sec, meta->ctime.value.nsec, ctime_str_buf, sizeof(ctime_str_buf), &res);
            if (res != MPTAR_OK) return res;
            ctime_str_len = mptar_strlen(ctime_str_buf);
            pax_ctime_size = mptar_pax_calculate_record_len((mptar_uint32)ctime_str_len + MPTAR_PAX_OVERHEAD_TIME);
            pax_header_size += pax_ctime_size;
        }
#endif

        if (uid_needs_pax) {
            mptar_u64toa(meta->uid, uid_str_buf, sizeof(uid_str_buf), &res);
            if (res != MPTAR_OK) return res;
            uid_str_len = mptar_strlen(uid_str_buf);
            pax_uid_size = mptar_pax_calculate_record_len((mptar_uint32)uid_str_len + MPTAR_PAX_OVERHEAD_UID);
            pax_header_size += pax_uid_size;
        }

        if (gid_needs_pax) {
            mptar_u64toa(meta->gid, gid_str_buf, sizeof(gid_str_buf), &res);
            if (res != MPTAR_OK) return res;
            gid_str_len = mptar_strlen(gid_str_buf);
            pax_gid_size = mptar_pax_calculate_record_len((mptar_uint32)gid_str_len + MPTAR_PAX_OVERHEAD_GID);
            pax_header_size += pax_gid_size;
        }

        if (uname_needs_pax) {
            pax_uname_size = mptar_pax_calculate_record_len((mptar_uint32)uname_len + MPTAR_PAX_OVERHEAD_UNAME);
            pax_header_size += pax_uname_size;
        }

        if (gname_needs_pax) {
            pax_gname_size = mptar_pax_calculate_record_len((mptar_uint32)gname_len + MPTAR_PAX_OVERHEAD_GNAME);
            pax_header_size += pax_gname_size;
        }

        res = mptar_write_pax_header(ctx, pax_header_size, meta);
        if (res != MPTAR_OK) return res;

        if (large_path) {
            res = mptar_pax_stream_record(ctx, pax_path_size, "path", meta->path, path_len);
            if (res != MPTAR_OK) return res;
        }

        if (large_size) {
            res = mptar_pax_stream_record(ctx, pax_size_size, "size", size_str_buf, size_str_len);
            if (res != MPTAR_OK) return res;
        }

        if (large_link_target) {
            res = mptar_pax_stream_record(ctx, pax_link_target_size, "linkpath", meta->link_target, link_target_size);
            if (res != MPTAR_OK) return res;
        }

        if (mtime_needs_pax) {
            res = mptar_pax_stream_record(ctx, pax_mtime_size, "mtime", mtime_str_buf, mtime_str_len);
            if (res != MPTAR_OK) return res;
        }
        
#ifdef MPTAR_SUPPORT_EXTRA_TIMES
        if (meta->atime.has_value) {
            res = mptar_pax_stream_record(ctx, pax_atime_size, "atime", atime_str_buf, atime_str_len);
            if (res != MPTAR_OK) return res;
        }

        if (meta->ctime.has_value) {
            res = mptar_pax_stream_record(ctx, pax_ctime_size, "ctime", ctime_str_buf, ctime_str_len);
            if (res != MPTAR_OK) return res;
        }
#endif

        if (uid_needs_pax) {
            res = mptar_pax_stream_record(ctx, pax_uid_size, "uid", uid_str_buf, uid_str_len);
            if (res != MPTAR_OK) return res;
        }

        if (gid_needs_pax) {
            res = mptar_pax_stream_record(ctx, pax_gid_size, "gid", gid_str_buf, gid_str_len);
            if (res != MPTAR_OK) return res;
        }

        if (uname_needs_pax) {
            res = mptar_pax_stream_record(ctx, pax_uname_size, "uname", meta->uname, uname_len);
            if (res != MPTAR_OK) return res;
        }

        if (gname_needs_pax) {
            res = mptar_pax_stream_record(ctx, pax_gname_size, "gname", meta->gname, gname_len);
            if (res != MPTAR_OK) return res;
        }

        mptar_size_t remainder = (mptar_size_t)(pax_header_size % MPTAR_BLOCK_SIZE);
        mptar_size_t padding_needed = (remainder > 0) ? (MPTAR_BLOCK_SIZE - remainder) : 0;
        if (padding_needed > 0) {
            res = mptar_stream_write_zeroes(ctx, padding_needed);
            if (res != MPTAR_OK) return res;
        }
    }

    char* header_bytes = (char*)ctx->memory.alloc(ctx->memory.alloc_user_data, MPTAR_USTAR_HEADER_SIZE);
    if (header_bytes == MPTAR_NULL) {
        return MPTAR_ERR_ALLOC;
    }

    mptar_header* header = (mptar_header*)header_bytes;
    mptar_write_ustar_header(header, meta);

    mptar_size_t written = ctx->write(ctx->write_user_data, header_bytes, MPTAR_USTAR_HEADER_SIZE);
    ctx->memory.free(ctx->memory.alloc_user_data, header_bytes);

    if (written != MPTAR_USTAR_HEADER_SIZE) {
        return MPTAR_ERR_IO_WRITE;
    }

    ctx->bytes_left = meta->size;

    return MPTAR_OK;    
}

mptar_size_t mptar_write_data_chunk(mptar_writer *ctx, const void *buffer, mptar_size_t size, int *out_err)
{
    if(out_err) *out_err = MPTAR_OK;
    if (size == 0 || ctx->bytes_left == 0) return 0;
    if (!ctx || !buffer) {
        if (out_err) *out_err = MPTAR_ERR_INVALID_ARG;
        return 0;
    }
    
    mptar_size_t bytes_to_write = size;
    if (bytes_to_write > ctx->bytes_left) {
        bytes_to_write = ctx->bytes_left;
    }

    mptar_size_t written = ctx->write(ctx->write_user_data, buffer, bytes_to_write);
    
    if (written == 0 || written > bytes_to_write) {
        if (out_err) *out_err = MPTAR_ERR_IO_WRITE;
        return 0; 
    }

    ctx->bytes_left -= written;
    return written;
}

int mptar_write_finalize(mptar_writer *ctx, const mptar_metadata *meta)
{
    if (!ctx || !meta) return MPTAR_ERR_INVALID_ARG;
    if (ctx->bytes_left > 0) {
        return MPTAR_ERR_INCOMPLETE_PAYLOAD;
    }

    mptar_size_t remainder = (mptar_size_t)(meta->size % MPTAR_BLOCK_SIZE);
    mptar_size_t padding_needed = (remainder > 0) ? (MPTAR_BLOCK_SIZE - remainder) : 0;

    if (padding_needed > 0) {
        int err = mptar_stream_write_zeroes(ctx, padding_needed);
        if (err != MPTAR_OK) {
            return err;
        }
    }

    return MPTAR_OK;
}

int mptar_close_archive(mptar_writer *ctx)
{
    if (!ctx) return MPTAR_ERR_INVALID_ARG;
    
    int err = mptar_stream_write_zeroes(ctx, MPTAR_EOA_MARKER_SIZE);
    if (err != MPTAR_OK) {
        return err;
    }

    return MPTAR_OK;
}

#endif /* MPTAR_NO_WRITE */

#ifndef MPTAR_NO_READ

#if defined(MPTAR_CUSTOM_ATOIU64)
    #define MPTAR_CUSTOM_ATOU64
    #define MPTAR_CUSTOM_ATOI64
#endif

#ifndef MPTAR_CUSTOM_ATOU64
/**
 * \brief Parses a string slice into an unsigned 64-bit integer with explicit length and overflow protection.
 * \details Safely converts a bounded ASCII string representation of a number into an unsigned 64-bit integer, 
 *          handling arithmetic overflows and checking for malformed input.
 * \note This function can be overridden by defining \c MPTAR_CUSTOM_ATOU64.
 * \param str Pointer to the character string buffer to parse.
 * \param len Number of characters to inspect in the string.
 * \param[out] err Output status pointer receiving an error code on failure (e.g., \ref MPTAR_ERR_OVERFLOW, \ref MPTAR_ERR_MALFORMED, or \ref MPTAR_ERR_INVALID_ARG).
 * \return The parsed 64-bit unsigned integer value, or a boundary fallback on error.
 */
static mptar_uint64 mptar_atou64(const char* str, mptar_size_t len, int* err) {
    if(err) *err = MPTAR_OK;
    if(str == MPTAR_NULL || len == 0){
        if(err) *err = MPTAR_ERR_INVALID_ARG;
        return 0;
    }

    const unsigned char* bytes = (const unsigned char*)str;
    mptar_uint64 val = 0;
    mptar_size_t digits_parsed = 0;

    mptar_size_t i = 0;
    for ( ; i < len; i++) {
        unsigned char c = bytes[i];
        if (c < '0' || c > '9') break;

        if (val > (MPTAR_UINT64_MAX / 10)) {
            if(err) *err = MPTAR_ERR_OVERFLOW;
            return MPTAR_UINT64_MAX;
        }

        mptar_uint64 next_val = val * 10;
        unsigned char digit = c - '0';

        if (next_val > (MPTAR_UINT64_MAX - digit)) {
            if(err) *err = MPTAR_ERR_OVERFLOW;
            return MPTAR_UINT64_MAX;
        }

        val = next_val + digit;
        digits_parsed++;
    }

    if(digits_parsed == 0){
        if(err) *err = MPTAR_ERR_MALFORMED;
        return 0;
    }

    return val;
}

#else

extern mptar_uint64 mptar_atou64(const char* str, mptar_size_t len, int* err);

#endif

#ifndef MPTAR_CUSTOM_ATOI64
/**
 * \brief Parses a string slice into a signed 64-bit integer with sign detection, length bounds, and overflow protection.
 * \details Safely converts a bounded ASCII string representation (optionally prefixed with '+' or '-') into a signed 64-bit integer, 
 *          handling arithmetic overflows and checking for malformed input.
 * \note This function can be overridden by defining \c MPTAR_CUSTOM_ATOI64.
 * \param str Pointer to the character string buffer to parse.
 * \param len Number of characters to inspect in the string.
 * \param[out] err Output status pointer receiving an error code on failure (e.g., \ref MPTAR_ERR_OVERFLOW, \ref MPTAR_ERR_MALFORMED, or \ref MPTAR_ERR_INVALID_ARG).
 * \return The parsed 64-bit signed integer value, or a boundary fallback on error.
 */
static mptar_int64 mptar_atoi64(const char* str, mptar_size_t len, int* err) {
    if (err) *err = MPTAR_OK;
    if (str == MPTAR_NULL || len == 0) {
        if(err) *err = MPTAR_ERR_INVALID_ARG;
        return 0;
    }

    bool is_negative = false;
    mptar_size_t start_idx = 0;

    if (str[0] == '-') {
        is_negative = true;
        start_idx = 1;
    } else if (str[0] == '+') {
        start_idx = 1;
    }

    if (start_idx >= len) {
        if (err) *err = MPTAR_ERR_MALFORMED;
        return 0;
    }

    int atou_error = 0;
    mptar_uint64 u_val = mptar_atou64(str + start_idx, len - start_idx, &atou_error);
    if (err) *err = atou_error;
    if (atou_error != MPTAR_OK && atou_error != MPTAR_ERR_OVERFLOW) return 0;

    if (is_negative) {
        mptar_uint64 max_negative_abs = (mptar_uint64)MPTAR_INT64_MAX + 1ULL;

        if (atou_error == MPTAR_ERR_OVERFLOW || u_val >= max_negative_abs) {
            return MPTAR_INT64_MIN;
        }

        return -(mptar_int64)u_val;
    } else {
        if (atou_error == MPTAR_ERR_OVERFLOW || u_val > (mptar_uint64)MPTAR_INT64_MAX) {
            return MPTAR_INT64_MAX;
        }

        return (mptar_int64)u_val;
    }
}

#else

extern mptar_int64 mptar_atoi64(const char* str, mptar_size_t len, int* err);

#endif

#ifndef MPTAR_CUSTOM_CONSUME_STREAM
/**
 * \brief Consumes and discards a specified number of bytes from the reader's input stream.
 * \details Reads chunks of data into a temporary stack buffer until the requested count 
 *          of bytes has been skipped or the underlying stream reaches EOF.
 * \note This function can be overridden by defining \c MPTAR_CUSTOM_CONSUME_STREAM.
 * \warning Overriding this function carries no stability guarantees; 
 *          it may be modified or removed entirely in future versions 
 *          without notice or deprecation.
 * \param reader Pointer to the initialized \ref mptar_reader context structure.
 * \param count Total number of bytes to consume and discard.
 * \return The actual number of bytes successfully consumed and discarded.
 */
static mptar_size_t mptar_consume_stream(mptar_reader* reader, mptar_size_t count) {
    if (count == 0) return 0;

    char discard_buf[128];
    mptar_size_t total_consumed = 0;

    while (total_consumed < count) {
        mptar_size_t remaining = count - total_consumed;
        mptar_size_t chunk = (remaining > sizeof(discard_buf)) ? sizeof(discard_buf) : remaining;

        mptar_size_t bytes_read = reader->read(reader->read_user_data, discard_buf, chunk);
        if (bytes_read == 0) {
            break;
        }

        total_consumed += bytes_read;
        reader->offset += bytes_read;
    }

    return total_consumed;
}
#else
extern mptar_size_t mptar_consume_stream(mptar_reader* reader, mptar_size_t count);
#endif

#ifndef MPTAR_CUSTOM_ALIGN_STREAM_BY_DISCARD
/**
 * \brief Aligns the reader stream to the next 512-byte block boundary by actively reading and discarding padding bytes.
 * \details Checks current block alignment via the reader offset and reads/discards any trailing block 
 *          padding bytes required to reach the start of the next tar block boundary.
 * \note This function can be overridden by defining \c MPTAR_CUSTOM_ALIGN_STREAM_BY_DISCARD.
 * \warning Overriding this function carries no stability guarantees; 
 *          it may be modified or removed entirely in future versions 
 *          without notice or deprecation.
 * \param reader Pointer to the initialized \ref mptar_reader context structure.
 * \return \ref MPTAR_OK on success, or \ref MPTAR_ERR_IO_READ if the required padding bytes could not be fully read.
 */
static int mptar_align_stream_by_discard(mptar_reader* reader) {
    if (reader->bytes_left != 0) return MPTAR_OK;

    mptar_size_t remainder = reader->offset % MPTAR_BLOCK_SIZE;
    if (remainder > 0) {
        mptar_size_t padding_needed = MPTAR_BLOCK_SIZE - remainder;
        mptar_size_t skipped = mptar_consume_stream(reader, padding_needed);
        
        if (skipped < padding_needed) {
            return MPTAR_ERR_IO_READ;
        }
    }
    return MPTAR_OK;
}
#else
extern int mptar_align_stream_by_discard(mptar_reader* reader);
#endif

#ifndef MPTAR_CUSTOM_ALIGN_STREAM_BY_SKIP
/**
 * \brief Aligns the reader stream to the next 512-byte block boundary by adjusting the tracking offset.
 * \details Adjusts the reader offset forward to account for block alignment padding without 
 *          performing physical read/discard operations on the underlying stream.
 * \note This function can be overridden by defining \c MPTAR_CUSTOM_ALIGN_STREAM_BY_SKIP.
 * \warning Overriding this function carries no stability guarantees; 
 *          it may be modified or removed entirely in future versions 
 *          without notice or deprecation.
 * \param reader Pointer to the initialized \ref mptar_reader context structure.
 * \return \ref MPTAR_OK on success.
 */
static int mptar_align_stream_by_skip(mptar_reader* reader) {
    if (reader->bytes_left != 0) return MPTAR_OK;

    mptar_size_t remainder = reader->offset % MPTAR_BLOCK_SIZE;
    if (remainder > 0) {
        mptar_size_t padding_needed = MPTAR_BLOCK_SIZE - remainder;
        reader->offset += padding_needed;
    }
    return MPTAR_OK;
}
#else
extern int mptar_align_stream_by_skip(mptar_reader* reader);
#endif

#ifndef MPTAR_CUSTOM_PARSE_OCTAL_FIELD
/**
 * \brief Parses a numeric field from a tar header block, supporting standard octal text or binary formats.
 * \details Handles both traditional POSIX octal ASCII representation (with space/zero padding) and standard 
 *          binary-encoded extensions (indicated by the high-order bit set) into an unsigned 64-bit integer.
 * \note This function can be overridden by defining \c MPTAR_CUSTOM_PARSE_OCTAL_FIELD.
 * \warning Overriding this function carries no stability guarantees; 
 *          it may be modified or removed entirely in future versions 
 *          without notice or deprecation.
 * \param str Pointer to the character array buffer containing the field.
 * \param str_size Number of bytes/characters in the field.
 * \param err Optional pointer to an integer variable to receive error or success status codes (e.g., \ref MPTAR_OK, \ref MPTAR_ERR_INVALID_ARG, \ref MPTAR_ERR_MALFORMED, or \ref MPTAR_ERR_OVERFLOW).
 * \return The parsed 64-bit unsigned integer value, or an error boundary limit on failure.
 */
static mptar_uint64 mptar_parse_octal_field(const char* str, mptar_size_t str_size, int* err) {    
    if (str == MPTAR_NULL || str_size == 0) {
        if(err) *err = MPTAR_ERR_INVALID_ARG;
        return 0;
    }

    const unsigned char* bytes = (const unsigned char*)str;

    if((bytes[0] & MPTAR_OCTAL_BINARY_FLAG) != 0){
        mptar_uint64 result = 0;
        result = bytes[0] & 0x7F;

        mptar_size_t i = 1;
        for ( ; i < str_size; i++)
        {
            if (result > (MPTAR_UINT64_MAX >> 8)) {
                if (err) *err = MPTAR_ERR_OVERFLOW;
                return MPTAR_UINT64_MAX;
            }
            
            result = (result << 8) | bytes[i];
        }

        if (err) *err = MPTAR_OK;
        return result;
    }

    mptar_uint64 result = 0;
    mptar_size_t i = 0;

    while (i < str_size && (bytes[i] == ' ' || bytes[i] == '0')) {
        i++;
    }

    while (i < str_size) {
        unsigned char c = bytes[i];

        if (c == '\0' || c == ' ') {
            break;
        }

        if (c < '0' || c > '7') {
            if(err) *err = MPTAR_ERR_MALFORMED;
            return result; 
        }

        if (result > (MPTAR_UINT64_MAX >> 3)) {
            if(err) *err = MPTAR_ERR_OVERFLOW;
            return MPTAR_UINT64_MAX; 
        }

        result = (result << 3) + (c - '0');
        i++;
    }

    while (i < str_size) {
        unsigned char c = bytes[i];
        if (c != '\0' && c != ' ') {
            if(err) *err = MPTAR_ERR_MALFORMED;
            return result; 
        }
        i++;
    }

    if(err) *err = MPTAR_OK;
    return result;
}
#else
extern mptar_uint64 mptar_parse_octal_field(const char* str, mptar_size_t str_size, int* err);
#endif

#ifndef MPTAR_CUSTOM_VERIFY_HEADER_CHECKSUM
/**
 * \brief Verifies the checksum of a parsed POSIX tar archive header block.
 * \details Computes both unsigned and signed byte sums across the 512-byte header block (treating 
 *          the checksum field itself as spaces) and compares it against the stored octal checksum value. 
 *          Also detects the standard end-of-archive (EOA) zero-block marker.
 * \note This function can be overridden by defining \c MPTAR_CUSTOM_VERIFY_HEADER_CHECKSUM.
 * \warning Overriding this function carries no stability guarantees; 
 *          it may be modified or removed entirely in future versions 
 *          without notice or deprecation.
 * \param header Pointer to the target \ref mptar_header structure to verify.
 * \return \ref MPTAR_OK on valid checksum, \ref MPTAR_EOF if an end-of-archive marker block is detected, or an error code (e.g., \ref MPTAR_ERR_INVALID_ARG or \ref MPTAR_ERR_CHECKSUM).
 */
static int mptar_verify_header_checksum(const mptar_header* header) {
    if(header == MPTAR_NULL) return MPTAR_ERR_INVALID_ARG;

    const unsigned char* bytes = (const unsigned char*)header;

    mptar_uint32 stored_checksum = (mptar_uint32)mptar_parse_octal_field(header->checksum, sizeof(header->checksum), MPTAR_NULL);
    if (header->typeflag == '\0' && stored_checksum == 0 && header->name[0] == '\0') {
        static const char zero_magic_version[sizeof(header->magic) + sizeof(header->version)] = {0};
        if (mptar_memcmp(header->magic, zero_magic_version, sizeof(header->magic) + sizeof(header->version)) == 0) {
            return MPTAR_EOF;
        }
    }

    mptar_uint32 unsigned_sum = 0;
    mptar_int32 signed_sum = 0;

    mptar_uint16 i = 0;
    for ( ; i < MPTAR_CHECKSUM_OFFSET; i++) {
        unsigned_sum += bytes[i];
        signed_sum += (signed char)bytes[i];
    }

    /* Hardcoded constant value for the 8 space characters (8 * 32) */
    unsigned_sum += MPTAR_SPACE_CHECKSUM_VAL; 
    signed_sum += MPTAR_SPACE_CHECKSUM_VAL;

    for (i = MPTAR_CHECKSUM_END_OFFSET; i < MPTAR_USTAR_HEADER_SIZE; i++) {
        unsigned_sum += bytes[i];
        signed_sum += (signed char)bytes[i];
    }

    if (unsigned_sum == stored_checksum || (mptar_uint32)signed_sum == stored_checksum) {
        return MPTAR_OK;
    }

    return MPTAR_ERR_CHECKSUM;
}
#else
extern int mptar_verify_header_checksum(const mptar_header* header);
#endif

#ifndef MPTAR_CUSTOM_PARSE_PAX_TIME
/**
 * \brief Parses a PAX extended timestamp string containing seconds and optional fractional nanoseconds.
 * \details Splits a PAX time string around the decimal point, parses the seconds portion into 
 *          signed 64-bit integers using \ref mptar_atoi64, and processes up to standard sub-second decimal precision 
 *          digits into nanoseconds.
 * \note This function can be overridden by defining \c MPTAR_CUSTOM_PARSE_PAX_TIME.
 * \warning Overriding this function carries no stability guarantees; 
 *          it may be modified or removed entirely in future versions 
 *          without notice or deprecation.
 * \param val_str Pointer to the character buffer containing the PAX time string.
 * \param val_len Length of the string in bytes.
 * \param out_time Pointer to the destination \ref mptar_opt_time structure to populate.
 * \return \ref MPTAR_OK on success, or an error code (e.g., \ref MPTAR_ERR_INVALID_ARG or \ref MPTAR_ERR_MALFORMED).
 */
static int mptar_parse_pax_time(const char* val_str, mptar_size_t val_len, mptar_opt_time* out_time) {
    if (val_str == MPTAR_NULL || out_time == MPTAR_NULL || val_len == 0) {
        return MPTAR_ERR_INVALID_ARG;
    }

    const unsigned char* bytes = (const unsigned char*)val_str;
    mptar_size_t dot_i = 0;
    while (dot_i < val_len && bytes[dot_i] != '.') {
        dot_i++;
    }

    int status = MPTAR_OK;
    out_time->value.sec = mptar_atoi64(val_str, dot_i, &status);
    out_time->value.nsec = 0;
    out_time->has_value = true;

    if (dot_i < val_len && bytes[dot_i] == '.') {
        mptar_size_t frac_start = dot_i + 1;
        mptar_uint32 multiplier = MPTAR_PAX_NSEC_INITIAL_MULTIPLIER;
        mptar_uint32 nsec = 0;
        mptar_size_t i = frac_start;

        while (i < val_len && (i - frac_start) < MPTAR_PAX_NSEC_MAX_DIGITS) {
            unsigned char c = bytes[i];
            if (c < '0' || c > '9') {
                status = (status == MPTAR_OK) ? MPTAR_ERR_MALFORMED : status;
                out_time->value.nsec = nsec;
                return status;
            }

            nsec += (mptar_uint32)(c - '0') * multiplier;
            multiplier /= 10;
            i++;
        }

        while (i < val_len) {
            unsigned char c = bytes[i];
            if (c < '0' || c > '9') {
                status = (status == MPTAR_OK) ? MPTAR_ERR_MALFORMED : status;
                out_time->value.nsec = nsec;
                return status;
            }
            i++;
        }

        out_time->value.nsec = nsec;
    }

    return status;
}
#else
extern int mptar_parse_pax_time(const char* val_str, mptar_size_t val_len, mptar_opt_time* out_time);
#endif

#ifndef MPTAR_CUSTOM_RECONSTRUCT_USTAR_PATH
/**
 * \brief Reconstructs a full file path from a USTAR header's prefix and name fields.
 * \details Allocates memory using the reader's allocator to combine the optional directory prefix 
 *          and file name fields separated by a forward slash into a single null-terminated string path.
 * \note This function can be overridden by defining \c MPTAR_CUSTOM_RECONSTRUCT_USTAR_PATH.
 * \warning Overriding this function carries no stability guarantees; 
 *          it may be modified or removed entirely in future versions 
 *          without notice or deprecation.
 * \param reader Pointer to the initialized \ref mptar_reader context structure.
 * \param header Pointer to the source \ref mptar_header structure.
 * \param out_meta Pointer to the destination \ref mptar_metadata structure where the allocated path string will be stored.
 * \return \ref MPTAR_OK on success, or an error code (e.g., \ref MPTAR_ERR_INVALID_ARG, \ref MPTAR_ERR_ALLOC, or \ref MPTAR_ERR_MALFORMED).
 */
static int mptar_reconstruct_ustar_path(mptar_reader* reader, const mptar_header* header, mptar_metadata* out_meta) {
    if(reader == MPTAR_NULL || header == MPTAR_NULL || out_meta == MPTAR_NULL) return MPTAR_ERR_INVALID_ARG;
    out_meta->path = MPTAR_NULL;
    
    mptar_size_t name_len = mptar_strnlen(header->name, sizeof(header->name));
    mptar_size_t prefix_len = mptar_strnlen(header->prefix, sizeof(header->prefix));

    if (prefix_len > 0) {
        mptar_size_t full_len = prefix_len + 1 + name_len; /* 1 for a forward slash that joins the prefix and name */
        char* allocated_path = (char*)reader->memory.alloc(reader->memory.alloc_user_data, full_len + 1);
        if (!allocated_path) return MPTAR_ERR_ALLOC;

        mptar_memcpy(allocated_path, header->prefix, prefix_len);
        allocated_path[prefix_len] = '/';
        mptar_memcpy(allocated_path + prefix_len + 1, header->name, name_len);
        allocated_path[full_len] = '\0';
        out_meta->path = allocated_path;
    } else if (name_len > 0) {
        char* allocated_path = (char*)reader->memory.alloc(reader->memory.alloc_user_data, name_len + 1);
        if (!allocated_path) return MPTAR_ERR_ALLOC;

        mptar_memcpy(allocated_path, header->name, name_len);
        allocated_path[name_len] = '\0';
        out_meta->path = allocated_path;
    }else{
        return MPTAR_ERR_MALFORMED;
    }
    
    return MPTAR_OK;
}
#else
extern int mptar_reconstruct_ustar_path(mptar_reader* reader, const mptar_header* header, mptar_metadata* out_meta);
#endif

#ifndef MPTAR_CUSTOM_ALLOC_PAX_STRING
/**
 * \brief Allocates and copies a null-terminated string buffer for PAX record values.
 * \details Uses the provided memory configuration allocator to allocate space for the source string 
 *          plus a null terminator, copies the content, and ensures safety against overflow.
 * \note This function can be overridden by defining \c MPTAR_CUSTOM_ALLOC_PAX_STRING.
 * \warning Overriding this function carries no stability guarantees; 
 *          it may be modified or removed entirely in future versions 
 *          without notice or deprecation.
 * \param mem Pointer to the \ref mptar_memory_cfg configuration structure containing allocators.
 * \param src Pointer to the source string bytes.
 * \param len Number of bytes to copy from the source string.
 * \return Pointer to the newly allocated null-terminated string, or \ref MPTAR_NULL on failure.
 */
static const char* mptar_alloc_pax_string(mptar_memory_cfg* mem, const char* src, mptar_size_t len) {
    if (mem == MPTAR_NULL || src == MPTAR_NULL || len >= (mptar_size_t)-1) {
        return MPTAR_NULL;
    }

    char* dst = (char*)mem->alloc(mem->alloc_user_data, len + 1);
    if (!dst) return MPTAR_NULL;

    mptar_memcpy(dst, src, len);
    dst[len] = '\0';

    return dst;
}
#else
extern const char* mptar_alloc_pax_string(mptar_memory_cfg* mem, const char* src, mptar_size_t len);
#endif

#ifndef MPTAR_CUSTOM_APPLY_PAX_STRING
/**
 * \brief Applies a parsed PAX string attribute to a destination string pointer based on precedence rules.
 * \details Handles either 'last-wins' overwrites (freeing any prior allocated string) or 'first-wins' behavior 
 *          according to active macro compilation definitions, updating respective PAX flags.
 * \note This function can be overridden by defining \c MPTAR_CUSTOM_APPLY_PAX_STRING.
 * \warning Overriding this function carries no stability guarantees; 
 *          it may be modified or removed entirely in future versions 
 *          without notice or deprecation.
 * \param mem Pointer to the \ref mptar_memory_cfg configuration structure.
 * \param val Pointer to the source value string buffer.
 * \param val_len Length of the value string in bytes.
 * \param out_str Pointer to the destination string pointer to update.
 * \param pax_flags Pointer to the bitfield tracking applied PAX flags.
 * \param flag Specific bit flag representing the PAX attribute being applied.
 */
static void mptar_apply_pax_string(mptar_memory_cfg* mem, const char* val, mptar_size_t val_len, const char** out_str, mptar_uint32* pax_flags, mptar_uint32 flag) {
#ifdef MPTAR_PAX_LAST_WINS
    if (*out_str != MPTAR_NULL && mem->free) {
        mem->free(mem->alloc_user_data, (void*)*out_str);
        *out_str = MPTAR_NULL;
    }
#else
    if (*pax_flags & flag) return;
#endif

    const char* allocated = mptar_alloc_pax_string(mem, val, val_len);
    if (allocated != MPTAR_NULL) {
        *out_str = allocated;
        *pax_flags |= flag;
    } else {
        *pax_flags &= ~flag; 
    }
}
#else
extern void mptar_apply_pax_string(mptar_memory_cfg* mem, const char* val, mptar_size_t val_len, const char** out_str, mptar_uint32* pax_flags, mptar_uint32 flag);
#endif

#ifndef MPTAR_CUSTOM_APPLY_PAX_U64
/**
 * \brief Parses and applies a 64-bit unsigned integer PAX attribute value.
 * \details Converts a string into an unsigned 64-bit integer using \ref mptar_atou64 and updates the target variable 
 *          and tracking flags according to precedence policy.
 * \note This function can be overridden by defining \c MPTAR_CUSTOM_APPLY_PAX_U64.
 * \warning Overriding this function carries no stability guarantees; 
 *          it may be modified or removed entirely in future versions 
 *          without notice or deprecation.
 * \param val Pointer to the source value string buffer.
 * \param val_len Length of the value string in bytes.
 * \param out_val Pointer to the destination 64-bit integer variable to update.
 * \param pax_flags Pointer to the bitfield tracking applied PAX flags.
 * \param flag Specific bit flag representing the PAX attribute being applied.
 */
static void mptar_apply_pax_u64(const char* val, mptar_size_t val_len, mptar_uint64* out_val, mptar_uint32* pax_flags, mptar_uint32 flag) {
#ifndef MPTAR_PAX_LAST_WINS
    if (*pax_flags & flag) return;
#endif

    int error = MPTAR_OK;
    mptar_uint64 parsed = mptar_atou64(val, val_len, &error);
    if (error == MPTAR_OK || error == MPTAR_ERR_OVERFLOW) {
        *out_val = parsed;
        *pax_flags |= flag;
    }
}
#else
extern void mptar_apply_pax_u64(const char* val, mptar_size_t val_len, mptar_uint64* out_val, mptar_uint32* pax_flags, mptar_uint32 flag);
#endif

#ifndef MPTAR_CUSTOM_APPLY_PAX_TIME
/**
 * \brief Parses and applies a PAX timestamp attribute value.
 * \details Parses timestamp strings via \ref mptar_parse_pax_time and updates the destination optional time structure 
 *          and tracking flags based on precedence policy.
 * \note This function can be overridden by defining \c MPTAR_CUSTOM_APPLY_PAX_TIME.
 * \warning Overriding this function carries no stability guarantees; 
 *          it may be modified or removed entirely in future versions 
 *          without notice or deprecation.
 * \param val Pointer to the source value string buffer.
 * \param val_len Length of the value string in bytes.
 * \param out_time Pointer to the destination \ref mptar_opt_time structure to update.
 * \param pax_flags Pointer to the bitfield tracking applied PAX flags.
 * \param flag Specific bit flag representing the PAX attribute being applied.
 */
static void mptar_apply_pax_time(const char* val, mptar_size_t val_len, mptar_opt_time* out_time, mptar_uint32* pax_flags, mptar_uint32 flag) {
#ifndef MPTAR_PAX_LAST_WINS
    if (*pax_flags & flag) return;
#endif

    int error = mptar_parse_pax_time(val, val_len, out_time);
    if (error == MPTAR_OK || error == MPTAR_ERR_MALFORMED) {
        *pax_flags |= flag;
    }
}
#else
extern void mptar_apply_pax_time(const char* val, mptar_size_t val_len, mptar_opt_time* out_time, mptar_uint32* pax_flags, mptar_uint32 flag);
#endif

#ifndef MPTAR_CUSTOM_APPLY_PAX_KV
/**
 * \brief Parses and applies a PAX extended header key-value pair to an entry's metadata structure.
 * \details Dispatches recognized PAX attribute keys (such as path, linkpath, size, uid, gid, mtime, 
 *          and optional extra timestamps) to their respective handlers, updating metadata fields and tracking flags.
 * \note This function can be overridden by defining \c MPTAR_CUSTOM_APPLY_PAX_KV.
 * \warning Overriding this function carries no stability guarantees; 
 *          it may be modified or removed entirely in future versions 
 *          without notice or deprecation.
 * \param reader Pointer to the initialized \ref mptar_reader context structure.
 * \param key Pointer to the PAX record key string.
 * \param key_len Length of the key string in bytes.
 * \param val Pointer to the PAX record value string.
 * \param val_len Length of the value string in bytes.
 * \param meta Pointer to the destination \ref mptar_metadata structure to update.
 * \param pax_flags Pointer to the bitfield tracking applied PAX flags.
 */
static void mptar_apply_pax_kv(mptar_reader* reader, const char* key, mptar_size_t key_len, 
    const char* val, mptar_size_t val_len, mptar_metadata* meta, mptar_uint32* pax_flags) {
    if (!reader || !key || !val || !meta || !pax_flags) {
        return;
    }
    
    if (key_len == 4 && mptar_memcmp(key, "path", 4) == 0) {
        mptar_apply_pax_string(&reader->memory, val, val_len, &meta->path, pax_flags, MPTAR_PAX_HAS_PATH);
    } 
    else if (key_len == 8 && mptar_memcmp(key, "linkpath", 8) == 0) {
        mptar_apply_pax_string(&reader->memory, val, val_len, &meta->link_target, pax_flags, MPTAR_PAX_HAS_LINK);
    }
    else if (key_len == 5 && mptar_memcmp(key, "uname", 5) == 0) {
        mptar_apply_pax_string(&reader->memory, val, val_len, &meta->uname, pax_flags, MPTAR_PAX_HAS_UNAME);
    }
    else if (key_len == 5 && mptar_memcmp(key, "gname", 5) == 0) {
        mptar_apply_pax_string(&reader->memory, val, val_len, &meta->gname, pax_flags, MPTAR_PAX_HAS_GNAME);
    }
    else if (key_len == 4 && mptar_memcmp(key, "size", 4) == 0) {
        mptar_apply_pax_u64(val, val_len, &meta->size, pax_flags, MPTAR_PAX_HAS_SIZE);
    } 
    else if (key_len == 3 && mptar_memcmp(key, "uid", 3) == 0) {
        mptar_apply_pax_u64(val, val_len, &meta->uid, pax_flags, MPTAR_PAX_HAS_UID);
    } 
    else if (key_len == 3 && mptar_memcmp(key, "gid", 3) == 0) {
        mptar_apply_pax_u64(val, val_len, &meta->gid, pax_flags, MPTAR_PAX_HAS_GID);
    }
    else if (key_len == 5 && mptar_memcmp(key, "mtime", 5) == 0) {
        mptar_apply_pax_time(val, val_len, &meta->mtime, pax_flags, MPTAR_PAX_HAS_MTIME);
    }
#ifdef MPTAR_SUPPORT_EXTRA_TIMES
    else if (key_len == 5 && mptar_memcmp(key, "atime", 5) == 0) {
        mptar_apply_pax_time(val, val_len, &meta->atime, pax_flags, MPTAR_PAX_HAS_ATIME);
    } 
    else if (key_len == 5 && mptar_memcmp(key, "ctime", 5) == 0) {
        mptar_apply_pax_time(val, val_len, &meta->ctime, pax_flags, MPTAR_PAX_HAS_CTIME);
    }
#endif
}
#else
extern void mptar_apply_pax_kv(mptar_reader* reader, const char* key, mptar_size_t key_len, 
    const char* val, mptar_size_t val_len, mptar_metadata* meta, mptar_uint32* pax_flags);
#endif

#ifndef MPTAR_CUSTOM_PARSE_PAX_BLOCK
/**
 * \brief Parses a PAX extended header data block containing sequential key-value records.
 * \details Allocates memory to read the full PAX block payload from the input stream, 
 *          consumes any trailing block alignment padding, parses individual space-separated 
 *          length records (`len key=val\n`), and applies them via PAX attribute handlers.
 * \note This function can be overridden by defining \c MPTAR_CUSTOM_PARSE_PAX_BLOCK.
 * \warning Overriding this function carries no stability guarantees; 
 *          it may be modified or removed entirely in future versions 
 *          without notice or deprecation.
 * \param reader Pointer to the initialized \ref mptar_reader context structure.
 * \param total_pax_size Total byte length of the PAX extended header block payload.
 * \param meta Pointer to the target \ref mptar_metadata structure to populate.
 * \param pax_flags Pointer to the bitfield tracking applied PAX flags.
 * \return \ref MPTAR_OK on success, or an error code (e.g., \ref MPTAR_ERR_INVALID_ARG, \ref MPTAR_ERR_OVERFLOW, \ref MPTAR_ERR_ALLOC, \ref MPTAR_ERR_IO_READ, or \ref MPTAR_ERR_MALFORMED).
 */
static int mptar_parse_pax_block(mptar_reader* reader, mptar_uint64 total_pax_size, mptar_metadata* meta, mptar_uint32* pax_flags) {
    if (!reader || !meta || !pax_flags) return MPTAR_ERR_INVALID_ARG;
    if (total_pax_size == 0) return MPTAR_OK;

    mptar_size_t size_t_pax = (mptar_size_t)total_pax_size;
    if (size_t_pax != total_pax_size || size_t_pax > MPTAR_MAX_PAX_SIZE) {
        return MPTAR_ERR_OVERFLOW;
    }

    char* buffer = (char*)reader->memory.alloc(reader->memory.alloc_user_data, size_t_pax);
    if (!buffer) return MPTAR_ERR_ALLOC;

    if (reader->read(reader->read_user_data, buffer, size_t_pax) != size_t_pax) {
        reader->memory.free(reader->memory.alloc_user_data, buffer);
        return MPTAR_ERR_IO_READ;
    }
    reader->offset += size_t_pax;

    /* Reads remaining block padding */
    mptar_uint32 remainder = total_pax_size % MPTAR_BLOCK_SIZE;
    if (remainder > 0) {
        char dummy[MPTAR_USTAR_HEADER_SIZE];
        mptar_size_t pad = MPTAR_BLOCK_SIZE - (mptar_size_t)remainder;
        mptar_size_t read = reader->read(reader->read_user_data, dummy, pad);
        if (read != pad) {
            reader->memory.free(reader->memory.alloc_user_data, buffer);
            return MPTAR_ERR_IO_READ;
        }
        reader->offset += pad;
    }

    int res = MPTAR_OK;
    mptar_size_t i = 0;
    while (i < size_t_pax) {
        mptar_size_t len_start = i;
        while (i < size_t_pax && buffer[i] != ' ') {
            i++;
        }

        if (i >= size_t_pax) {
            res = MPTAR_ERR_MALFORMED;
            goto error_cleanup;
        }
        
        mptar_uint64 record_len = mptar_atou64(&buffer[len_start], i - len_start, &res);
        if (res != MPTAR_OK) {
            goto error_cleanup;
        } else if (record_len > (mptar_uint64)size_t_pax || record_len == 0) {
            res = MPTAR_ERR_MALFORMED;
            goto error_cleanup;
        }

        i++; /* Advances past ' ' */

        mptar_size_t key_start = i;
        while (i < size_t_pax && buffer[i] != '=') {
            i++;
        }

        if (i >= size_t_pax) {
            res = MPTAR_ERR_MALFORMED;
            goto error_cleanup;
        }
        
        mptar_size_t key_len = i - key_start;
        i++; /* Advances past '=' */

        mptar_size_t val_start = i;
        mptar_size_t target_next_record = len_start + (mptar_size_t)record_len;

        if (target_next_record > size_t_pax || target_next_record <= val_start) {
            res = MPTAR_ERR_MALFORMED;
            goto error_cleanup;
        } else if (buffer[target_next_record - 1] != '\n') {
            res = MPTAR_ERR_MALFORMED;
            goto error_cleanup;
        }

        mptar_size_t val_len = target_next_record - val_start - 1;
        mptar_apply_pax_kv(reader, &buffer[key_start], key_len, &buffer[val_start], val_len, meta, pax_flags);
        
        i = target_next_record;
    }

    reader->memory.free(reader->memory.alloc_user_data, buffer);
    return MPTAR_OK;

error_cleanup:
    reader->memory.free(reader->memory.alloc_user_data, buffer);
    mptar_reader_free_metadata(reader, meta);
    return res;
}
#else
extern int mptar_parse_pax_block(mptar_reader* reader, mptar_uint64 total_pax_size, mptar_metadata* meta, mptar_uint32* pax_flags);
#endif

#ifndef MPTAR_CUSTOM_PARSE_USTAR_HEADER
/**
 * \brief Parses standard base USTAR archive header fields into an entry metadata structure.
 * \details Extracts and validates standard file attributes (such as size, typeflag, mode, user/group IDs, 
 *          modification time, and optional special device numbers) from a raw parsed header block.
 * \note This function can be overridden by defining \c MPTAR_CUSTOM_PARSE_USTAR_HEADER.
 * \warning Overriding this function carries no stability guarantees; 
 *          it may be modified or removed entirely in future versions 
 *          without notice or deprecation.
 * \param header Pointer to the source \ref mptar_header structure.
 * \param out_meta Pointer to the destination \ref mptar_metadata structure to populate.
 * \return \ref MPTAR_OK on success, or an error code (e.g., \ref MPTAR_ERR_INVALID_ARG or field parsing errors).
 */
static int mptar_parse_ustar_header(const mptar_header* header, mptar_metadata* out_meta) {
    if (!header || !out_meta) return MPTAR_ERR_INVALID_ARG;

    int error = MPTAR_OK;

    out_meta->size = mptar_parse_octal_field(header->size, sizeof(header->size), &error);
    if (error != MPTAR_OK) return error;

    out_meta->typeflag = header->typeflag;

    error = MPTAR_OK;
    out_meta->mode = (mptar_uint32)mptar_parse_octal_field(header->mode, sizeof(header->mode), &error);
    if (error != MPTAR_OK) {
        out_meta->mode = (header->typeflag == '5') ? MPTAR_MODE_DIR : MPTAR_MODE_REG;
    }

    error = MPTAR_OK;
    out_meta->uid = mptar_parse_octal_field(header->uid, sizeof(header->uid), &error);
    if (error != MPTAR_OK) {
        out_meta->uid = 0;
    }

    error = MPTAR_OK;
    out_meta->gid = mptar_parse_octal_field(header->gid, sizeof(header->gid), &error);
    if (error != MPTAR_OK) {
        out_meta->gid = 0;
    }
    
    error = MPTAR_OK;
    out_meta->mtime.value.sec = (mptar_int64)mptar_parse_octal_field(header->mtime, sizeof(header->mtime), &error);
    out_meta->mtime.value.nsec = 0;
    if (error != MPTAR_OK) {
        out_meta->mtime.value.sec = 0;
    }
    out_meta->mtime.has_value = true;

#ifdef MPTAR_SUPPORT_EXTRA_TIMES
    out_meta->atime.has_value = false;
    out_meta->ctime.has_value = false;
#endif

#ifdef MPTAR_SUPPORT_SPECIAL
    error = MPTAR_OK;
    out_meta->devmajor = (mptar_uint32)mptar_parse_octal_field(header->devmajor, sizeof(header->devmajor), &error);
    if (error != MPTAR_OK) {
        out_meta->devmajor = 0;
    }

    error = MPTAR_OK;
    out_meta->devminor = (mptar_uint32)mptar_parse_octal_field(header->devminor, sizeof(header->devminor), &error);
    if (error != MPTAR_OK) {
        out_meta->devminor = 0;
    }
#endif
    
    return MPTAR_OK;
}
#else
extern int mptar_parse_ustar_header(const mptar_header* header, mptar_metadata* out_meta);
#endif

int mptar_read_header(mptar_reader *reader, mptar_metadata *out_meta) {
    if (!reader || !out_meta) return MPTAR_ERR_INVALID_ARG;

    out_meta->path = MPTAR_NULL;
    out_meta->link_target = MPTAR_NULL;
    out_meta->uname = MPTAR_NULL;
    out_meta->gname = MPTAR_NULL;
    out_meta->internal_alloc = true;

    mptar_uint32 pax_flags = 0;
    char header_bytes[MPTAR_USTAR_HEADER_SIZE];
    int res = MPTAR_OK;
    
    while (1) {
        if (reader->read(reader->read_user_data, header_bytes, MPTAR_USTAR_HEADER_SIZE) != MPTAR_USTAR_HEADER_SIZE) {
            res = MPTAR_ERR_IO_READ;
            goto error_cleanup;
        }

        reader->offset += MPTAR_USTAR_HEADER_SIZE;

        mptar_header* header = (mptar_header*)header_bytes;
        res = mptar_verify_header_checksum(header);
        if (res != MPTAR_OK) {
            goto error_cleanup;
        }

        if (header->typeflag == 'x') {
            res = MPTAR_OK;
            mptar_uint64 pax_size = mptar_parse_octal_field(header->size, sizeof(header->size), &res);
            if (res != MPTAR_OK) {
                goto error_cleanup;
            }

            res = mptar_parse_pax_block(reader, pax_size, out_meta, &pax_flags);
            if (res != MPTAR_OK) {
                goto error_cleanup;
            }

            continue;
        }

        if (header->typeflag != '\0' && (header->typeflag < '0' || header->typeflag > '7')) {
            res = MPTAR_OK;
            mptar_uint64 ext_size = mptar_parse_octal_field(header->size, sizeof(header->size), &res);
            
            if (res != MPTAR_OK) {
                /* We assume the extension we are trying to skip has no payload (size = 0)
                   We can skip it because we already loaded the 512B of the extension
                   The specification states if size is unsed the extension has no payload. */
                ext_size = 0;
                continue;
            }

            mptar_size_t remainder = (mptar_size_t)(ext_size % MPTAR_BLOCK_SIZE);
            mptar_size_t total_skip = (mptar_size_t)ext_size + ((remainder > 0) ? (MPTAR_BLOCK_SIZE - remainder) : 0);                        
            if (total_skip > 0) {
                mptar_size_t skipped = mptar_consume_stream(reader, total_skip);        
                if (skipped != total_skip) {
                    res = MPTAR_ERR_IO_READ;
                    goto error_cleanup;
                }
            }

            continue;
        }

        mptar_metadata pax_staged = *out_meta;
        res = mptar_parse_ustar_header(header, out_meta);
        if (res != MPTAR_OK) {
            goto error_cleanup;
        }

        if (pax_flags & MPTAR_PAX_HAS_SIZE)  { out_meta->size = pax_staged.size; }
        if (pax_flags & MPTAR_PAX_HAS_UID)   { out_meta->uid  = pax_staged.uid;  }
        if (pax_flags & MPTAR_PAX_HAS_GID)   { out_meta->gid  = pax_staged.gid;  }
        if (pax_flags & MPTAR_PAX_HAS_MTIME) { 
            out_meta->mtime = pax_staged.mtime; 
        }

#ifdef MPTAR_SUPPORT_EXTRA_TIMES
        if (pax_flags & MPTAR_PAX_HAS_ATIME) { out_meta->atime = pax_staged.atime; }
        if (pax_flags & MPTAR_PAX_HAS_CTIME) { out_meta->ctime = pax_staged.ctime; }
#endif

        if (pax_flags & MPTAR_PAX_HAS_PATH) {
            out_meta->path = pax_staged.path;
        } else {
            res = mptar_reconstruct_ustar_path(reader, header, out_meta);
            if (res != MPTAR_OK) {
                goto error_cleanup;
            }
        }

        if (pax_flags & MPTAR_PAX_HAS_LINK) {
            out_meta->link_target = pax_staged.link_target;
        } else {
            mptar_size_t link_len = mptar_strnlen(header->linkname, sizeof(header->linkname));
            if (link_len > 0) {
                char* allocated_link = (char*)reader->memory.alloc(reader->memory.alloc_user_data, link_len + 1);
                if (!allocated_link) {
                    res = MPTAR_ERR_ALLOC;
                    goto error_cleanup;
                }
                mptar_memcpy(allocated_link, header->linkname, link_len);
                allocated_link[link_len] = '\0';
                out_meta->link_target = allocated_link;
            }else{
                out_meta->uname = MPTAR_NULL;
            }            
        }

        if (pax_flags & MPTAR_PAX_HAS_UNAME) {
            out_meta->uname = pax_staged.uname;
        } else {
            mptar_size_t uname_len = mptar_strnlen(header->uname, sizeof(header->uname));
            if (uname_len > 0) {
                char* allocated_uname = (char*)reader->memory.alloc(reader->memory.alloc_user_data, uname_len + 1);
                if (!allocated_uname) {
                    res = MPTAR_ERR_ALLOC;
                    goto error_cleanup;
                }
                mptar_memcpy(allocated_uname, header->uname, uname_len);
                allocated_uname[uname_len] = '\0';
                out_meta->uname = allocated_uname;
            } else {
                out_meta->uname = MPTAR_NULL;
            }
        }

        if (pax_flags & MPTAR_PAX_HAS_GNAME) {
            out_meta->gname = pax_staged.gname;
        } else {
            mptar_size_t gname_len = mptar_strnlen(header->gname, sizeof(header->gname));
            if (gname_len > 0) {
                char* allocated_gname = (char*)reader->memory.alloc(reader->memory.alloc_user_data, gname_len + 1);
                if (!allocated_gname) {
                    res = MPTAR_ERR_ALLOC;
                    goto error_cleanup;
                }
                mptar_memcpy(allocated_gname, header->gname, gname_len);
                allocated_gname[gname_len] = '\0';
                out_meta->gname = allocated_gname;
            } else {
                out_meta->gname = MPTAR_NULL;
            }
        }

        reader->bytes_left = out_meta->size;
        break;
    }

    return MPTAR_OK;

error_cleanup:
    mptar_reader_free_metadata(reader, out_meta);
    return res;
}

mptar_size_t mptar_read_data_chunk(mptar_reader* reader, void* buffer, mptar_size_t size, int *out_err) {
    if (out_err) *out_err = MPTAR_OK;

    if (!reader || !buffer) {
        if (out_err) *out_err = MPTAR_ERR_INVALID_ARG;
        return 0;
    }

    int align_err = mptar_align_stream_by_discard(reader);
    if (align_err != MPTAR_OK) {
        if (out_err) *out_err = align_err;
        return 0;
    }

    if (size == 0 || reader->bytes_left == 0) return 0;

    mptar_size_t bytes_to_read = size;
    if (bytes_to_read > reader->bytes_left) {
        bytes_to_read = reader->bytes_left;
    }

    mptar_size_t bytes_read = reader->read(reader->read_user_data, buffer, bytes_to_read);
    if (bytes_read == 0 || bytes_read > bytes_to_read) {
        if (out_err) *out_err = MPTAR_ERR_IO_READ;
        return 0;
    }

    reader->bytes_left -= bytes_read;
    reader->offset += bytes_read;

    align_err = mptar_align_stream_by_discard(reader);
    if (align_err != MPTAR_OK && out_err) {
        *out_err = align_err;
    }

    return bytes_read;
}

int mptar_discard_data(mptar_reader* reader) {
    if (!reader) {
        return MPTAR_ERR_INVALID_ARG;
    }

    if (reader->bytes_left > 0) {
        mptar_size_t skipped = mptar_consume_stream(reader, reader->bytes_left);        
        reader->bytes_left -= skipped;
        
        if (skipped < reader->bytes_left + skipped) {
            return MPTAR_ERR_IO_READ; 
        }
    }

    int align_err = mptar_align_stream_by_discard(reader);
    if (align_err != MPTAR_OK) {
        return align_err;
    }

    return MPTAR_OK;
}

int mptar_skip_data(mptar_reader* reader) {
    if (!reader) {
        return MPTAR_ERR_INVALID_ARG;
    }

    if (reader->bytes_left > 0) {
        mptar_uint64 bytes_to_skip = reader->bytes_left;
        reader->offset += bytes_to_skip;
        reader->bytes_left = 0;   
    }

    mptar_align_stream_by_skip(reader);

    return MPTAR_OK;
}

void mptar_reader_free_metadata(mptar_reader *reader, mptar_metadata *meta)
{
    if (!reader || !meta || !meta->internal_alloc) return;

MPTAR_SUPPRESS_WARNING_CAST_QUAL_BEGIN /* Reason: metadata strings in reading api are allocated dynamically. */
    if (meta->path) {
        reader->memory.free(reader->memory.alloc_user_data, (void*)meta->path);
        meta->path = MPTAR_NULL;
    }
    if (meta->link_target) {
        reader->memory.free(reader->memory.alloc_user_data, (void*)meta->link_target);
        meta->link_target = MPTAR_NULL;
    }
    if (meta->uname) {
        reader->memory.free(reader->memory.alloc_user_data, (void*)meta->uname);
        meta->uname = MPTAR_NULL;
    }
    if (meta->gname) {
        reader->memory.free(reader->memory.alloc_user_data, (void*)meta->gname);
        meta->gname = MPTAR_NULL;
    }
    
    meta->internal_alloc = false;

MPTAR_SUPPRESS_WARNING_CAST_QUAL_END
}

#endif /* MPTAR_NO_READ */
