#include <windows.h>
#include "../shared/hooks.h"
#include "./modules/log.h"
#include "./modules/extractor/extractor.h"

/// @brief Main windows installer entry. Runs core modules. 
/// @return 0 if no errors.
int mainCRTStartup(){
    int stage = INIT;
    
    print("Starting the installation process...\n");
    extract("extracted"); // Temporary name - later configured by cli/user.

    ExitProcess(0);
    return 0;
}