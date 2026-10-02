#pragma once

#include "kstring.h"
#include "tomlc17.h"
#include <stdbool.h>

/**
 * @brief User manifest with package metadata and options passed to the Quarkit builder
 */
typedef struct {
    bool start_shortcut; /**< Should it create a shortcut in the start menu.s */
    bool admin_required; /**< Are administrator privileges required to install this. */

    /**
     * @brief A relative path to a directory where Quarkit will scan for triples (system, architecture, bitness)
     * after which it will package files according to \ref target_dir
     */
    kstring_t search_dir;
    kstring_t target_dir; /**< Sets the path to target directory relative to found directories. */
    kstring_t package_name; /**< Sets the package name. (e.g. Application name) */
    kstring_t launch_name; /**< Sets the path to an executable which should be launched, relative to package directory. */
    kstring_t output_path; /**< Sets the path to where the package should be outputted.  */

    toml_result_t internal; /**< Internal data structure of tomlc17 library. Needs to be freed with the qkit_free_manifest */
} qkit_manifest;

/**
 * @brief Converts provided enum value to a string.
 * @param type Enum value to convert.
 * @return Pointer to string representation of the enum value.
 */
const char* toml_type_to_string(enum toml_type_t type);

/**
 * @brief Prints the manifest in a pretty style - useful for debugging/logs.
 * @param manifest Manifest to print contents of.
 */
void qkit_print_manifest(const qkit_manifest* manifest);

/**
 * @brief Reads file at `path` and attempts to deserialize content of it to \ref qkit_manifest
 * @param manifest Pointer to \ref qkit_manifest which will be populated with deserialized contents. 
 * @param path Path to a file containing the manifest.
 * @return \ref QKIT_OK if succeeded, error code otherwise.
 */
int qkit_deserialize_manifest(qkit_manifest* manifest, char* path);

/**
 * @brief Frees the internal allocated memory of \ref qkit_manifest
 * @param manifest Pointer to the manifest which should be freed.
 */
void qkit_free_manifest(qkit_manifest* manifest);