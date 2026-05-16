#pragma once

#ifdef MODULE_LOGGING
  /// @brief Outputs a simple message to the console.
  /// @param msg Contents of the message.
  extern void print(const char* msg);
#else
  // Macro replacement that is removed from compilation.
  #define print(msg) ((void)0) 
#endif