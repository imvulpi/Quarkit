#pragma once
#include "kstring.h"
#include "kvec.h"

/**
 * @brief Represents supported target operating systems.
 */
typedef enum {
    QKIT_SYS_UNKNOWN = 0,
    QKIT_SYS_MACOS,
    QKIT_SYS_WINDOWS,
    QKIT_SYS_LINUX
} qkit_system;

/**
 * @brief Represents supported target CPU architectures.
 */
typedef enum {
    QKIT_ARCH_UNKNOWN = 0,
    QKIT_ARCH_X86,
    QKIT_ARCH_ARM,
    QKIT_ARCH_RISCV
} qkit_architecture;

/**
 * @brief Represents a target platform triplet (OS, bitness, architecture).
 */
typedef struct {
    qkit_system system;             /**< Target operating system */
    int bitness;                    /**< Target bitness (32 or 64) */
    qkit_architecture architecture; /**< Target CPU architecture */
} qkit_triple;

/**
 * @brief Represents a discovered payload item along with its triple metadata.
 */
typedef struct {
    qkit_triple* triple;        /**< Pointer to heap-allocated target triple metadata */
    kstring_t name;             /**< Filename stored as a klib kstring_t */
} qkit_discovery;

/**
 * @brief Dynamic vector of discovered payload pointers managed via klib (kvec).
 */
typedef kvec_t(qkit_discovery*) qkit_discovery_vec;

/**
 * @brief Converts a system enum value to its human-readable string representation.
 * 
 * @param[in] system Operating system enum value.
 * @return Static string representation (e.g., "macos", "windows", "linux", "unknown").
 */
const char* qkit_system_to_string(qkit_system system);

/**
 * @brief Converts an architecture enum value to its human-readable string representation.
 * 
 * @param[in] arch CPU architecture enum value.
 * @return Static string representation (e.g., "x86", "arm", "riscv", "unknown").
 */
const char* qkit_arch_to_string(qkit_architecture arch);

/**
 * @brief Formats a target triple into a standard triplet string (e.g., "x86_64-windows").
 * 
 * @param[in]  triple Pointer to the target triple struct.
 * @param[out] buf    Destination buffer to hold the formatted string.
 * @param[in]  size   Capacity of the destination buffer.
 */
void qkit_triple_to_string(const qkit_triple *triple, char *buf, size_t size);

/**
 * @brief Prints a single discovery object in a formatted block.
 * 
 * @param[in] discovery Pointer to the discovery instance to print.
 */
void qkit_print_discovery(const qkit_discovery *discovery);

/**
 * @brief Prints all discoveries contained within a vector.
 * 
 * @param[in] discovery_vec Pointer to the vector of discoveries.
 */
void qkit_print_discoveries(const qkit_discovery_vec *discovery_vec);

/**
 * @brief Detects operating system from a string.
 * 
 * @param[in] lower_name Lowercase string hint.
 * @return Detected qkit_system enum value.
 */
qkit_system qkit_detect_system(const char *lower_name);

/**
 * @brief Detects architecture and bitness from a string.
 * 
 * @param[in]  lower_name  Lowercase string hint.
 * @param[out] out_arch     Pointer to store detected architecture.
 * @param[out] out_bitness  Pointer to store detected bitness (32 or 64).
 */
void qkit_detect_arch_and_bitness(const char *lower_name, qkit_architecture *out_arch, int *out_bitness);


/**
 * @brief Detects target system, architecture, and bitness from a string, populating a triple struct.
 * 
 * Performs primary explicit detection for system and architecture, followed by 
 * context-aware implicit fallbacks (e.g., resolving 'win32' or 'win64' when arch is omitted,
 * or matching Go-style '386').
 * 
 * @param[in]  lower_name Lowercase string hint.
 * @param[out] out_triple Pointer to the qkit_triple struct to populate.
 */
void qkit_detect_triple(const char *lower_name, qkit_triple *out_triple);

/**
 * @brief Scans a directory path and discovers compatible payload directories/files.
 * 
 * Iterates through directory entries, identifies target triples from directory/file names,
 * and pushes matched discovery instances into the provided vector.
 * 
 * @param[in]     path        Path to the directory containing payload files/directories.
 * @param[in,out] discoveries Pointer to an initialized kvec vector where discovered
 *                            items will be pushed.
 * @return QKIT_OK on success, or QKIT_ERR if directory opening or memory allocation fails.
 */
int qkit_discover_payloads(const char *path, qkit_discovery_vec *discoveries);

/**
 * @brief Deallocates a single discovery instance and its internal members.
 * 
 * Frees the associated @c triple, destroys the @c name kstring, and frees the
 * @c discovery object itself.
 * 
 * @param[in,out] discovery Pointer to the discovery object to free. Safe to pass NULL.
 */
void qkit_discovery_free(qkit_discovery *discovery);

/**
 * @brief Deallocates all discovery items inside a vector and resets the vector.
 * 
 * Iterates through each discovery pointer in the vector, invokes qkit_discovery_free
 * on each item, and destroys the vector storage using kvec macros.
 * 
 * @param[in,out] discovery_vec Pointer to the kvec vector to free. Safe to pass NULL.
 */
void qkit_discovery_free_vec(qkit_discovery_vec *discovery_vec);