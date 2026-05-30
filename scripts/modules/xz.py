import os

from scripts.build_enums import Arch, System

from ..build_config import BuildConfig
from ..compiler import Compiler, make_compiler
from ..globals import MODULES_DIR

def build(config: BuildConfig):
    print("Building the XZ LZMA module")
    sources = [
        "modules/xz/deps/liblzma/common/stream_buffer_decoder.c", 
        "modules/xz/deps/liblzma/common/stream_decoder.c", 
        "modules/xz/deps/liblzma/common/common.c", 
        "modules/xz/deps/liblzma/common/index_hash.c", 
        "modules/xz/deps/liblzma/check/check.c", 
        "modules/xz/deps/liblzma/common/block_header_decoder.c", 
        "modules/xz/deps/liblzma/common/filter_decoder.c", 
        "modules/xz/deps/liblzma/common/stream_flags_decoder.c", 
        "modules/xz/deps/liblzma/common/block_decoder.c", 
        "modules/xz/deps/liblzma/common/filter_common.c", 
        "modules/xz/deps/liblzma/common/block_util.c", 
        "modules/xz/deps/liblzma/common/vli_decoder.c", 
        "modules/xz/deps/liblzma/common/filter_flags_decoder.c", 
        "modules/xz/deps/liblzma/common/stream_flags_common.c", 
        "modules/xz/deps/liblzma/common/vli_size.c", 
        "modules/xz/deps/liblzma/check/crc32_small.c", 
        "modules/xz/deps/liblzma/lzma/lzma2_decoder.c", 
        "modules/xz/deps/liblzma/lzma/lzma_decoder.c", 
        "modules/xz/deps/liblzma/lz/lz_decoder.c",
    ]

    if(config.target_system == System.WINDOWS):
        sources.append("installer/windows/include/windows_glue.c")
    elif(config.target_system == System.LINUX):
        sources.append("installer/linux/include/linux_glue.c")

    output_dir = f"{MODULES_DIR}xz/{config.get_triple()}"
    os.makedirs(output_dir, exist_ok=True)

    compiler: Compiler = make_compiler(config)
    compiler.add_sources(sources)
    compiler.add_compilation_options(["-Iinstaller/shared/include", "-DHAVE_SMALL", "-DHAVE_DECODER_LZMA2", "-DHAVE_CHECK_CRC32", "-DHAVE_DECODERS", "-DHAVE__BOOL", "-DGLUE_SHARED"])
    compiler.optimize_for_size()
    compiler.disable_stdlib()
    link_flags = ["-s", "-lkernel32"]
    if(config.target_arch == Arch.Arm):
        link_flags.append("-lclang_rt.builtins-arm")

    compiler.compile_shared(f"{output_dir}/liblzma.so", link_flags)

    # A separate compiler for static library
    compiler = make_compiler(config)
    compiler.add_sources(sources)
    compiler.add_compilation_options(["-Iinstaller/shared/include", "-DHAVE_SMALL", "-DHAVE_DECODER_LZMA2", "-DHAVE_CHECK_CRC32", "-DHAVE_DECODERS", "-DHAVE__BOOL"])
    compiler.optimize_for_size()
    compiler.compile_static(f"{output_dir}/liblzma.a")