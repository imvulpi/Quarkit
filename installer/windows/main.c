/* 
* Reasoing behind using `__declspec(dllimport)` instead of `windows.h`:
* For some reason when using `windows.h` the functions never get inlined even when it would be optimal for it to be inlined, (e.g. single call)
* This might be some issue with how big the windows header file is and with the compiler being confused on what the function does?
*
* With inlining the code, the binary is a bit smaller (32-48 bytes depends on spacings) because it doesn't include 
* UNWIND_INFO and _IMAGE_RUNTIME_FUNCTION_ENTRY in the assembly of the program. 
* 
* This is only an improvement if the function is called once or if it's small enough to where the inlining
* would result in smaller size than the 32-48 byte change.
*
* Note: I tested a setup where some files would use the `windows.h` and others __declspec(dllimport) but this results in a unified system where nothing gets 
* inlined by default unless using attributes to force it. This is probably since all the files get merged during compilers actions. 
* For optimal size the __declspec(dllimport) should be preferred over `windows.h`  
*/

#ifndef QUARKIT_MODULE_INITS
#define QUARKIT_MODULE_INITS
#endif

__declspec(dllimport) void ExitProcess(unsigned int code);

/// @brief Method that invokes module functions.
int mainCRTStartup(){  
    QUARKIT_MODULE_INITS
    ExitProcess(0);
    return 0;
}