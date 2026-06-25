#pragma once
#ifdef QUARKIT_LOGGING_MODULE
  /// @brief Outputs a simple message to the console.
  /// @param msg Contents of the message.
  extern void print(const char* msg);
#else
  // Macro replacement that is removed from compilation.
  extern void print(const char* msg);
  // #define print(msg) ((void)0) 
#endif