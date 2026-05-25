import os

from ..build_config import BuildConfig
from ..compiler import Compiler, make_compiler
from ..globals import MODULES_DIR

def build(config: BuildConfig):
    print("Building the XZ LZMA module")
    sources = [
        "modules/xz/liblzma/common/stream_buffer_decoder.c", 
        "modules/xz/liblzma/common/stream_decoder.c", 
        "modules/xz/liblzma/common/common.c", 
        "modules/xz/liblzma/common/index_hash.c", 
        "modules/xz/liblzma/check/check.c", 
        "modules/xz/liblzma/common/block_header_decoder.c", 
        "modules/xz/liblzma/common/filter_decoder.c", 
        "modules/xz/liblzma/common/stream_flags_decoder.c", 
        "modules/xz/liblzma/common/block_decoder.c", 
        "modules/xz/liblzma/common/filter_common.c", 
        "modules/xz/liblzma/common/block_util.c", 
        "modules/xz/liblzma/common/vli_decoder.c", 
        "modules/xz/liblzma/common/filter_flags_decoder.c", 
        "modules/xz/liblzma/common/stream_flags_common.c", 
        "modules/xz/liblzma/common/vli_size.c", 
        "modules/xz/liblzma/check/crc32_small.c", 
        "modules/xz/liblzma/lzma/lzma2_decoder.c", 
        "modules/xz/liblzma/lzma/lzma_decoder.c", 
        "modules/xz/liblzma/lz/lz_decoder.c"
    ]

    output_dir = f"{MODULES_DIR}/{config.target_system}_{config.target_arch}/xz/"
    os.makedirs(output_dir, exist_ok=True)
    output_file = f"{output_dir}/liblzma.so"

    compiler: Compiler = make_compiler(config)
    compiler.add_sources(sources)
    compiler.add_compilation_options(["-DHAVE_SMALL", "-DHAVE_DECODER_LZMA2", "-DHAVE_CHECK_CRC32", "-DHAVE_DECODERS", "-DHAVE__BOOL"])
    compiler.compile_shared(output_file)