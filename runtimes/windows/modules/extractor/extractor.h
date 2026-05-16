#pragma once

#ifdef MODULE_EXTRACTION
    /// @brief Extracts the embedded payload to a file.
    /// @param filename Name of the file it should extract to.
    extern void extract(const char* filename);
#else
  // Macro replacement that is removed from compilation.
  #define extract(filename) ((void)0) 
#endif