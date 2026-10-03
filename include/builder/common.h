#pragma once

#pragma once

#if defined(_WIN32)
    #include <direct.h>
    #include <process.h>
    #include <windows.h>
    #include <shellapi.h>
    #define getpid _getpid
    #define mkdir_single(path) _mkdir(path)
#else
    #include <ftw.h>
    #include <unistd.h>
    #include <sys/stat.h>
    #include <sys/types.h>
    #define mkdir_single(path) mkdir(path, 0755)
#endif

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include "builder/discovery.h"

#define QKIT_OK   0
#define QKIT_ERR -1
#define QKIT_MANIFEST_MISSING   -2
#define QKIT_DESERIALIZE_FAILED -3
#define STR_LEN(s) (s), (sizeof(s) - 1) /**< Creates a string + length pair from a string literal. Useful for bounded string passes. */

static bool file_exists(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file != NULL) {
        fclose(file);
        return true;
/**
 * @brief Converts POSIX-style forward slashes to Windows-style backslashes in-place.
 * 
 * Replaces every occurrence of '/' with '\\' within the provided buffer up to `len` bytes.
 * Does not check for null-termination beyond the specified length.
 * 
 * @param[in,out] buf Pointer to the path string buffer to modify.
 * @param[in]     len Number of characters in the buffer to process.
 */
static void qkit_path_to_windows_slashes(char *buf, size_t len)
{
    if (buf == NULL) return;

    for (size_t i = 0; i < len; i++) {
        if (buf[i] == '/') {
            buf[i] = '\\';
        }
    }
}

#if !defined(_WIN32)
/* Internal callback for POSIX nftw recursive delete */
static int qkit_unlink_cb(const char *fpath, const struct stat *sb, int tflag, struct FTW *ftwbuf)
{
    (void)sb;
    (void)tflag;
    (void)ftwbuf;
    return remove(fpath);
}
#endif

/**
 * @brief Recursively removes a directory and all files/subdirectories inside it.
 * 
 * @param[in] path Path to the directory to remove.
 * @return 0 on success, non-zero on error.
 */
static int qkit_remove_dir_recursive(const char *path)
{
    if (path == NULL || *path == '\0') return QKIT_ERR;

#if defined(_WIN32)
    /* SHFileOperation requires double null-terminated string buffer */
    size_t len = strlen(path);
    char *buf = (char *)malloc(len + 2);
    if (buf == NULL) return QKIT_ERR;

    memcpy(buf, path, len);
    buf[len] = '\0';
    buf[len + 1] = '\0';

    qkit_path_to_windows_slashes(buf, len);

    SHFILEOPSTRUCTA file_op = {
        .hwnd = NULL,
        .wFunc = FO_DELETE,
        .pFrom = buf,
        .pTo = NULL,
        .fFlags = FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_SILENT,
        .fAnyOperationsAborted = FALSE,
        .hNameMappings = NULL,
        .lpszProgressTitle = NULL
    };

    int result = SHFileOperationA(&file_op);
    free(buf);
    return result;
#else
    return nftw(path, qkit_unlink_cb, 64, FTW_DEPTH | FTW_PHYS);
#endif
}
static char* strnlwr(char *str, size_t n){
    for (size_t i = 0; i < n; i++) str[i] = tolower(str[i]);
    return str;
}

/**
 * Invokes an expression, stores the result in a local status variable,
 * and jumps to the cleanup label if it fails.
 */
#define TRY_GOTO(expr) do { \
    status = (expr); \
    if (status != QKIT_OK) goto cleanup; \
} while(0)

/**
 * Invokes an expression, stores the result in a local status, if fails, 
 * it prints a formatted error message to stderr, and jumps to the cleanup label.
 */
#define TRY_GOTO_PRINT(expr, fmt, ...) do { \
    status = (expr); \
    if (status != QKIT_OK) { \
        fprintf(stderr, fmt "\n", ##__VA_ARGS__); \
        goto cleanup; \
    } \
} while(0)

/**
 * Invokes an expression and immediately returns its exact status code 
 * if it fails.
 */
#define TRY_RET(expr) do { \
    int _status = (expr); \
    if (_status != QKIT_OK) return _status; \
} while(0)

/**
 * Invokes an expression, captures its status code, prints a formatted 
 * error message to stderr, and returns that exact status code.
 */
#define TRY_RET_PRINT(expr, fmt, ...) do { \
    int _status = (expr); \
    if (_status != QKIT_OK) { \
        fprintf(stderr, fmt "\n", ##__VA_ARGS__); \
        return _status; \
    } \
} while(0)

/**
 * Invokes an expression and returns a explicitly provided error value 
 * (instead of the expression's code) if it fails.
 */
#define TRY(expr, err_val) do { \
    if ((expr) != QKIT_OK) { \
        return (err_val); \
    } \
} while(0)

/**
 * Invokes an expression and returns a explicitly provided error value 
 * (instead of the expression's code) if it fails, along with a custom log.
 */
#define TRY_PRINT(expr, err_val, fmt, ...) do { \
    if ((expr) != QKIT_OK) { \
        fprintf(stderr, fmt "\n", ##__VA_ARGS__); \
        return (err_val); \
    } \
} while(0)

/**
 * Invokes a custom expression in an if statement, if the statement succeeds 
 * it returns a provided error value
*/
#define TRY_CUSTOM(expr, err_val) do { \
    if((expr)) { \
        return (err_val); \
    } \
} while(0)

/**
 * Invokes a custom expression in an if statement, if the statement succeeds 
 * it returns a provided error value and prints a formatted error message to stderr.
*/
#define TRY_CUSTOM_PRINT(expr, err_val, fmt, ...) do { \
    if((expr)) { \
        fprintf(stderr, fmt "\n", ##__VA_ARGS__); \
        return (err_val); \
    } \
} while(0)
