#include "tomlc17.h"
#include <builder/manifest.h>
#include <builder/common.h>
#include <stdio.h>

const char* toml_type_to_string(enum toml_type_t type) 
{
    switch (type) {
        case TOML_UNKNOWN:     return "unknown";
        case TOML_STRING:      return "string";
        case TOML_INT64:       return "integer";
        case TOML_FP64:        return "float";
        case TOML_BOOLEAN:     return "boolean";
        case TOML_DATE:        return "date";
        case TOML_TIME:        return "time";
        case TOML_DATETIME:    return "datetime";
        case TOML_DATETIMETZ:  return "datetime with timezone";
        case TOML_ARRAY:       return "array";
        case TOML_TABLE:       return "table";
        default:               return "invalid type";
    }
}

void qkit_print_manifest(const qkit_manifest *manifest)
{
    printf("\n");
    printf("========================================\n");
    printf(" Loaded Manifest Configuration:\n");
    printf("========================================\n");
    printf("  * start_shortcut : %s\n", manifest->start_shortcut ? "true" : "false");
    printf("  * admin_required : %s\n", manifest->admin_required ? "true" : "false");
    printf("  * search_dir     : %s\n", manifest->search_dir.s);
    printf("  * target_dir     : %s\n", manifest->target_dir.s);
    printf("  * package_name   : %s\n", manifest->package_name.s);
    printf("  * launch_name    : %s\n", manifest->launch_name.s);
    printf("  * output_path    : %s\n", manifest->output_path.s);
    printf("========================================\n\n");
}

static int deserialize_field(toml_datum_t* table, const char* field_key, toml_type_t field_type, void* var, bool optional)
{
    toml_datum_t data = toml_seek(*table, field_key);

    if (data.type != field_type) {
        if (optional) {
            if (data.type != TOML_UNKNOWN) {
                fprintf(stderr, 
                    "[WARNING] Optional TOML field '%s' is not of an acceptable type (expected: %s, got: %s).\n", 
                    field_key, 
                    toml_type_to_string(field_type), 
                    toml_type_to_string(data.type)
                );
            }
            var = NULL;
            return QKIT_OK;
        }
        
        fprintf(stderr, 
            "[ERROR] Required TOML field '%s' is missing or has an invalid type (expected: %s, got: %s).\n", 
            field_key, 
            toml_type_to_string(field_type), 
            toml_type_to_string(data.type)
        );
        return QKIT_ERR;
    }

    switch (data.type) {
        case TOML_STRING:
            kputs((char*)data.u.s, var);
            break; 
        case TOML_BOOLEAN:
            *(bool*)var = data.u.boolean;
            break;
        case TOML_INT64:
            *(int64_t*)var = data.u.int64;
            break;
        case TOML_FP64:
            *(double*)var = data.u.fp64;
            break;
        case TOML_UNKNOWN:
            var = NULL;
            break;
        case TOML_DATE:
        case TOML_TIME:
        case TOML_DATETIME:
        case TOML_DATETIMETZ:
        case TOML_ARRAY:
        case TOML_TABLE:
            fprintf(stderr, 
                "[ERROR] Encountered unsupported TOML datatype for field '%s' (got: %s).\n"
                " -> Please check your manifest structure or refer to documentation.\n", 
                field_key,
                toml_type_to_string(data.type)
            );
            return QKIT_ERR;
        default:
            fprintf(stderr, 
                "[ERROR] Unknown or invalid TOML datatype encountered for field '%s' (code: %d).\n"
                " -> Please verify your configuration file syntax.\n", 
                field_key,
                data.type
            );
            return QKIT_ERR;
    }

    return QKIT_OK;
}

int qkit_deserialize_manifest(qkit_manifest* manifest, char* path){
    toml_result_t toml = toml_parse_file_ex(path);
    TRY_CUSTOM_PRINT(
        !toml.ok, 
        QKIT_DESERIALIZE_FAILED, 
        "[ERROR] Unexpected error while parsing TOML: %s\n", 
        toml.errmsg
    );

    qkit_manifest temp = {0};
    int status = QKIT_OK;
    TRY_GOTO(deserialize_field(&toml.toptab, "start_shortcut", TOML_BOOLEAN, &temp.start_shortcut, false));
    TRY_GOTO(deserialize_field(&toml.toptab, "admin_required", TOML_BOOLEAN, &temp.admin_required, true));
    TRY_GOTO(deserialize_field(&toml.toptab, "search_dir", TOML_STRING, &temp.search_dir, true));
    TRY_GOTO(deserialize_field(&toml.toptab, "target_dir", TOML_STRING, &temp.target_dir, true));
    TRY_GOTO(deserialize_field(&toml.toptab, "package_name", TOML_STRING,&temp.package_name, true));
    TRY_GOTO(deserialize_field(&toml.toptab, "launch_name", TOML_STRING, &temp.launch_name, true));
    TRY_GOTO(deserialize_field(&toml.toptab, "output_path", TOML_STRING, &temp.output_path, true));

    temp.internal = toml;
    *manifest = temp;
    return QKIT_OK;

    cleanup:
        qkit_free_manifest(&temp);
        toml_free(toml);
        return status;
}

void qkit_free_manifest(qkit_manifest* manifest){
    free(manifest->search_dir.s);
    free(manifest->target_dir.s);
    free(manifest->package_name.s);
    free(manifest->launch_name.s);
    free(manifest->output_path.s);
    toml_free(manifest->internal);
}
