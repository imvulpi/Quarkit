# Finds an appropriate compiler, if Enviromental CC is set prefer it.
if(NOT CMAKE_C_COMPILER)
    if(DEFINED ENV{CC})
        set(CMAKE_C_COMPILER $ENV{CC})
    else()
        find_program(CLANG_EXECUTABLE NAMES clang clang-cl)
        if(CLANG_EXECUTABLE)
            set(CMAKE_C_COMPILER ${CLANG_EXECUTABLE})
        else()
            find_program(GCC_CROSS_EXECUTABLE NAMES ${TOOLCHAIN_PREFIX}-gcc ${TARGET_TRIPLE}-gcc)
            if(GCC_CROSS_EXECUTABLE)
                set(CMAKE_C_COMPILER ${GCC_CROSS_EXECUTABLE})
            endif()
            # If nothing is found here, CMake attempts to find a standard default compiler.
        endif()
    endif()
endif()

# Clang and LLD cross-compilation.
if(CMAKE_C_COMPILER MATCHES "clang" AND NOT CMAKE_C_COMPILER MATCHES "clang-cl")
    set(CMAKE_C_COMPILER_TARGET ${TARGET_TRIPLE})
    set(CMAKE_C_FLAGS_INIT "--target=${TARGET_TRIPLE} ${CMAKE_C_FLAGS_INIT}")

    if(CMAKE_CROSSCOMPILING) # Prefer LLD when cross-compiling
        find_program(LLD_EXECUTABLE NAMES lld ld.lld)
        if(LLD_EXECUTABLE)
            set(CMAKE_EXE_LINKER_FLAGS_INIT "-fuse-ld=lld ${CMAKE_EXE_LINKER_FLAGS_INIT}")
            set(CMAKE_SHARED_LINKER_FLAGS_INIT "-fuse-ld=lld ${CMAKE_SHARED_LINKER_FLAGS_INIT}")
        endif()
    endif()
endif()