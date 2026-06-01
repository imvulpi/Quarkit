// Tool the modules uses to create compressed packages using lzma2
// This is not included in the installer.

#include "./deps/liblzma/common/block_encoder.h"
#include "./deps/liblzma/common/core/lzma12.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

/**
 * Compresses a file from input_path and writes it to output_path 
 * using the advanced lzma_stream_buffer_encode API.
 */
int compress_file(const char* input_path, const char* output_path) {
    FILE* in_file = fopen(input_path, "rb");
    if (!in_file) {
        fprintf(stderr, "Error: Could not open input file '%s'\n", input_path);
        return -1;
    }

    fseek(in_file, 0, SEEK_END);
    size_t in_size = ftell(in_file);
    rewind(in_file);

    uint8_t* in_buffer = (uint8_t*)malloc(in_size);
    if (in_size > 0 && !in_buffer) {
        fprintf(stderr, "Error: Memory allocation failed for input buffer\n");
        fclose(in_file);
        return -1;
    }

    if (in_size > 0) {
        fread(in_buffer, 1, in_size, in_file);
    }
    fclose(in_file);

    size_t out_size = in_size + (in_size / 8) + 128;
    uint8_t* out_buffer = (uint8_t*)malloc(out_size);
    if (!out_buffer) {
        fprintf(stderr, "Error: Memory allocation failed for output buffer\n");
        free(in_buffer);
        return -1;
    }

    size_t out_pos = 0;

    lzma_ret ret = lzma_easy_buffer_encode(
        6, // 1-9 
        LZMA_CHECK_CRC32,
        NULL,
        in_buffer, in_size,
        out_buffer, &out_pos,
        out_size
    );

    if (ret != LZMA_OK) {
        fprintf(stderr, "Error: Compression failed with code %d\n", ret);
        free(in_buffer);
        free(out_buffer);
        return -1;
    }

    FILE* out_f = fopen(output_path, "wb");
    if (!out_f) {
        fprintf(stderr, "Error: Could not open output file '%s' for writing\n", output_path);
        free(in_buffer);
        free(out_buffer);
        return -1;
    }

    fwrite(out_buffer, 1, out_pos, out_f);
    fclose(out_f);

    printf("Success! Compressed %zu bytes into %zu bytes.\n", in_size, out_pos);

    // Clean up allocated memory
    free(in_buffer);
    free(out_buffer);
    return 0;
}

int main(int argc, char** argv) {
    if (argc < 3) {
        printf("Usage: %s <input_file> <output_file>\n", argv[0]);
        return 1;
    }

    if (compress_file(argv[1], argv[2]) != 0) {
        return 1;
    }

    return 0;
}