#pragma once

#include <stdbool.h>
#include <stdio.h>

#define QKIT_OK   0
#define QKIT_ERR -1
#define QKIT_MANIFEST_MISSING   -2
#define QKIT_DESERIALIZE_FAILED -3
#define STR_LEN(s) (s), (sizeof(s) - 1) /**< Creates a string + length pair from a string literal. Useful for bounded string passes. */

bool file_exists(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file != NULL) {
        fclose(file);
        return true;
    }
    return false;
}

/**
 * Invokes an expression, stores the result in a local status variable,
 * and jumps to the error label if it fails.
 */
#define TRY_GOTO(expr) do { \
    status = (expr); \
    if (status != QKIT_OK) goto error; \
} while(0)

/**
 * Invokes an expression, stores the result in a local status, if fails, 
 * it prints a formatted error message to stderr, and jumps to the error label.
 */
#define TRY_GOTO_PRINT(expr, fmt, ...) do { \
    status = (expr); \
    if (status != QKIT_OK) { \
        fprintf(stderr, fmt "\n", ##__VA_ARGS__); \
        goto error; \
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
