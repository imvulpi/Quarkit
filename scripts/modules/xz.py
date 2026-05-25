import os

from ..build_config import BuildConfig
from ..compiler import Compiler, make_compiler
from ..globals import MODULES_DIR

def build(config: BuildConfig):
    print("Building the XZ LZMA module")
    sources = [
        "deps/xz/liblzma/common/stream_buffer_decoder.c", 
        "deps/xz/liblzma/common/stream_decoder.c", 
        "deps/xz/liblzma/common/common.c", 
        "deps/xz/liblzma/common/index_hash.c", 
        "deps/xz/liblzma/check/check.c", 
        "deps/xz/liblzma/common/block_header_decoder.c", 
        "deps/xz/liblzma/common/filter_decoder.c", 
        "deps/xz/liblzma/common/stream_flags_decoder.c", 
        "deps/xz/liblzma/common/block_decoder.c", 
        "deps/xz/liblzma/common/filter_common.c", 
        "deps/xz/liblzma/common/block_util.c", 
        "deps/xz/liblzma/common/vli_decoder.c", 
        "deps/xz/liblzma/common/filter_flags_decoder.c", 
        "deps/xz/liblzma/common/stream_flags_common.c", 
        "deps/xz/liblzma/common/vli_size.c", 
        "deps/xz/liblzma/check/crc32_small.c", 
        "deps/xz/liblzma/lzma/lzma2_decoder.c", 
        "deps/xz/liblzma/lzma/lzma_decoder.c", 
        "deps/xz/liblzma/lz/lz_decoder.c"
    ]

    output_dir = f"{MODULES_DIR}/{config.target_system}_{config.target_arch}/xz/"
    os.makedirs(output_dir, exist_ok=True)
    output_file = f"{output_dir}/liblzma.so"

    compiler: Compiler = make_compiler(config)
    compiler.add_sources(sources)
    compiler.add_compilation_options(["-DHAVE_SMALL", "-DHAVE_DECODER_LZMA2", "-DHAVE_CHECK_CRC32", "-DHAVE_DECODERS", "-DHAVE__BOOL"])
    compiler.compile_shared(output_file)