#include "builder/common.h"
#include <builder/discovery.h>
#include <builder/manifest.h>

#define QKIT_CLI_OPTION_NONE 0
#define QKIT_CLI_OPTION_MANIFEST 1

void qkit_print_header(void)
{
    printf("\n");
    printf("========================================\n");
    printf("  QUARKIT - Installer Generator\n");
    printf("========================================\n\n");
}

void print_help(){
    printf("QUARKIT - Installer Generator\nUSAGE: quarkit.exe [options]\nOPTIONS:\n  --manifest <path>    Provides path to the quarkit manifest.\n");
}

int main(int argc, char *argv[])
{
    qkit_print_header();
    char* manifest_path = "quarkit.toml";
    if(argc > 1){
        int previous_option = QKIT_CLI_OPTION_NONE;
        for (size_t i = 1; i < argc; i++){
            char* arg_str = argv[i];
            if(previous_option == QKIT_CLI_OPTION_MANIFEST) {
                manifest_path = arg_str;
                continue;
            }
            
            previous_option = QKIT_CLI_OPTION_NONE;
            
            if(strncmp(arg_str, STR_LEN("-h")) == 0
            || strncmp(arg_str, STR_LEN("--help")) == 0
            || strncmp(arg_str, STR_LEN("help")) == 0){
                print_help();
                return QKIT_OK;
            }else if(strncmp(arg_str, STR_LEN("--manifest")) == 0){
                previous_option = QKIT_CLI_OPTION_MANIFEST;
            }else{
                printf("Unknown option: %s\n", arg_str);
            }
        }
    }
    
    TRY_CUSTOM_PRINT(
        !file_exists(manifest_path), 
        QKIT_MANIFEST_MISSING, 
        "[ERROR] Manifest file not found or unreadable: %s\n"
        " -> Please ensure you are launching the executable from the correct working directory.\n", 
        manifest_path
    );

    qkit_manifest manifest = {0};
    TRY_PRINT(
        qkit_deserialize_manifest(&manifest, manifest_path), 
        QKIT_DESERIALIZE_FAILED, 
        "[ERROR] Failed to deserialize manifest: %s\n"
        " -> Please review the error details above.\n", 
        manifest_path
    );

    printf("[OK] Successfully loaded manifest from: %s\n", manifest_path);    
    qkit_print_manifest(&manifest);

    qkit_discovery_vec discoveries = {0};
    qkit_discover_payloads(manifest.payload_dir.s, &discoveries);
    qkit_print_discoveries(&discoveries);

    return QKIT_OK;
}