# Checks if APPLE_SDK_PATH is set or sets it if on Apple computer.
if(NOT DEFINED APPLE_SDK_PATH)
    if(DEFINED ENV{APPLE_SDK_PATH})
        set(APPLE_SDK_PATH $ENV{APPLE_SDK_PATH})
    elseif(CMAKE_HOST_APPLE)
        execute_process(
            COMMAND xcrun --sdk macosx --show-sdk-path
            OUTPUT_VARIABLE APPLE_SDK_PATH
            OUTPUT_STRIP_TRAILING_WHITESPACE
            ERROR_QUIET
        )
    endif()
endif()

if(APPLE_SDK_PATH)
    set(CMAKE_OSX_SYSROOT "${APPLE_SDK_PATH}")
    set(CMAKE_C_FLAGS_INIT "${CMAKE_C_FLAGS_INIT} -isysroot ${APPLE_SDK_PATH}")
    set(CMAKE_CXX_FLAGS_INIT "${CMAKE_CXX_FLAGS_INIT} -isysroot ${APPLE_SDK_PATH}")
endif()