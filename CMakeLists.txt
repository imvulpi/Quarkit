cmake_minimum_required(VERSION 3.15)
project(Quarkit C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Vendored Paths & Sources
    set(VENDOR_DIR ${CMAKE_CURRENT_SOURCE_DIR}/vendor)
    set(TOMLC17_SRC ${VENDOR_DIR}/tomlc17/tomlc17.c)
    set(MINIPAXTAR_SRC ${VENDOR_DIR}/minipaxtar/minipaxtar.c)
    set(MINIZ_SRC ${VENDOR_DIR}/miniz/miniz.c)
    set(LZ4_SRC ${VENDOR_DIR}/lz4/lib/lz4.c)
    set(BUILDER_VENDOR_INCLUDES
        ${VENDOR_DIR}/tomlc17
        ${VENDOR_DIR}/minipaxtar
        ${VENDOR_DIR}/miniz
        ${VENDOR_DIR}/lz4
        ${VENDOR_DIR}/zstd/lib
    )
# END - Vendored Paths & Sources

# Zstd Setup
    set(ZSTD_BUILD_PROGRAMS OFF CACHE BOOL "" FORCE)
    set(ZSTD_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(ZSTD_BUILD_SHARED OFF CACHE BOOL "" FORCE)
    set(ZSTD_BUILD_STATIC ON CACHE BOOL "" FORCE)

    # We can also make it compile .o files, but it would require listing the .c files
    # Which might not be desired - Maintaince pain? >_<
    add_subdirectory(${VENDOR_DIR}/zstd/build/cmake zstd_build)
# END - Zstd Setup

# Runtime API (Object Library)
    add_library(qkit_runtime OBJECT
        runtime/runtime.c
        ${MINIPAXTAR_SRC}
        ${MINIZ_SRC}
        ${LZ4_SRC}
    )

    target_compile_definitions(qkit_runtime PRIVATE 
        ZSTD_NO_COMPRESSOR=1 # We dont need compression for runtime
    )

    target_include_directories(qkit_runtime PRIVATE
        ./include/runtime
        ${VENDOR_DIR}/minipaxtar
        ${VENDOR_DIR}/miniz
        ${VENDOR_DIR}/lz4
        ${VENDOR_DIR}/zstd/lib
    )

    target_link_libraries(qkit_runtime PRIVATE libzstd_static)
# END - Runtime API

# Builder API (Shared Library)
    add_library(qkit_builder SHARED
        ${TOMLC17_SRC}
        ${MINIPAXTAR_SRC}
        ${MINIZ_SRC}
        ${LZ4_SRC}
    )

    target_include_directories(qkit_builder PRIVATE
        ./include/builder
        ${BUILDER_VENDOR_INCLUDES}
    )

    target_link_libraries(qkit_builder PRIVATE libzstd_static)

    if(WIN32)
        target_compile_definitions(qkit_builder PRIVATE BUILDER_EXPORTS)
    endif()
# END - Builder API

# Builder Executable
    add_executable(quarkit
        builder/main.c
    )

    target_include_directories(quarkit PRIVATE
        builder
        ${BUILDER_VENDOR_INCLUDES}
    )

    # Link against the builder shared library (DLL) and zstd
    target_link_libraries(quarkit PRIVATE 
        qkit_builder 
        libzstd_static
    )
# END - Builder Executable