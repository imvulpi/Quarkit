#pragma once

#ifdef MODULE_LOGGING
    extern void print(const char* msg);
#else
  // Macro replacement that is removed from compilation.
  #define print(msg) ((void)0) 
#endif