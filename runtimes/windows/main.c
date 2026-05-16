#include <windows.h>
#include "../shared/hooks.h"
#include "./modules/log.h"

#ifdef MODULE_EXTRACTION
/// @brief Extracts the embedded payload to a file.
/// @param filename Name of the file it should extract to.
extern void extract(const char* filename);
#endif

/// @brief Main windows installer entry. Runs core modules. 
/// @return 0 if no errors.
int mainCRTStartup(){
    int stage = INIT;
    
    print("Starting the installation process...\n");
    
    #ifdef MODULE_EXTRACTION
        extract("extracted"); // Temporary name - later configured by cli/user.
    #endif

    ExitProcess(0);
    return 0;
}