#pragma once

#include "kstring.h"
#include "tomlc17.h"
#include <stdbool.h>

/**
 * @brief User manifest containing package metadata and packaging rules.
 * 
 * Defines build configuration parsed from a TOML manifest file, instructing 
 * Quarkit on how to locate build outputs, target executables, and configure installer behavior.
 */
typedef struct {
    /** Whether to create a Start Menu shortcut on Windows during installation. */
    bool start_shortcut; 

    /** Whether elevated administrator privileges are required for installation. */
    bool admin_required; 

    /** Display name of the application package. */
    kstring_t package_name; 

    /** Relative path to the main application executable inside the packaged payload. */
    kstring_t main_executable; 

    /**
     * Relative path to the build output directory scanned for target triples 
     * (e.g., "bin/", "release/net9.0/"). Directories matching supported triples are packaged.
     */
    kstring_t payload_dir;

    /**
     * Relative path appended to each discovered triple directory to pinpoint 
     * the actual output payload (e.g., "publish/" or subdirectory containing executables).
     */
    kstring_t payload_subpath; 

    /** Installation directory path where package is unloaded - e.g. "{PROGRAM_FILES}/MyQkitApp" or "{LOCAL_APP_DATA}". */
    kstring_t install_path; 

    /** Destination directory path where final installer or archive artifacts are written */
    kstring_t artifact_dir;

    /** Internal TOML AST structure. Must be released by calling qkit_free_manifest(). */
    toml_result_t internal; 
} qkit_manifest;

/**
 * @brief Converts a TOML value type enum to its human-readable string representation.
 * 
 * @param[in] type TOML type enum value to convert.
 * @return Constant string representing the type name (e.g., "string", "int", "boolean").
 */
const char* toml_type_to_string(enum toml_type_t type);

/**
 * @brief Logs formatted contents of a manifest structure to stdout.
 * 
 * Helper for debugging and diagnostic logging.
 * 
 * @param[in] manifest Pointer to the populated manifest instance.
 */
void qkit_print_manifest(const qkit_manifest* manifest);

/**
 * @brief Parses a TOML manifest file from disk into a \ref qkit_manifest structure.
 * 
 * @param[out] manifest Pointer to the manifest struct to populate.
 * @param[in]  path     Path to the TOML manifest file on disk.
 * @return \ref QKIT_OK on successful parse, or a negative error code on failure.
 */
int qkit_deserialize_manifest(qkit_manifest* manifest, const char* path);

/**
 * @brief Frees all dynamically allocated memory within a \ref qkit_manifest instance.
 * 
 * Releases string memory and underlying TOML AST data held in \ref qkit_manifest.internal.
 * 
 * @param[in,out] manifest Pointer to the manifest instance to clean up.
 */
void qkit_free_manifest(qkit_manifest* manifest);