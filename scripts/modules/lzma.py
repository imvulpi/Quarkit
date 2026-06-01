import os
from scripts.build_enums import Arch, System

from ..build_config import BuildConfig
from ..compiler import Compiler, make_compiler
from ..globals import MODULES_DIR

LIBLZMA_LOCATION = "modules/lzma/deps/liblzma"
def build(config: BuildConfig):
    print("Building the XZ LZMA module")
    sources = [
        f"{LIBLZMA_LOCATION}/common/stream_buffer_decoder.c", 
        f"{LIBLZMA_LOCATION}/common/stream_decoder.c", 
        f"{LIBLZMA_LOCATION}/common/common.c", 
        f"{LIBLZMA_LOCATION}/common/index_hash.c", 
        f"{LIBLZMA_LOCATION}/check/check.c", 
        f"{LIBLZMA_LOCATION}/common/block_header_decoder.c", 
        f"{LIBLZMA_LOCATION}/common/filter_decoder.c",
        f"{LIBLZMA_LOCATION}/common/stream_flags_decoder.c", 
        f"{LIBLZMA_LOCATION}/common/block_decoder.c", 
        f"{LIBLZMA_LOCATION}/common/filter_common.c", 
        f"{LIBLZMA_LOCATION}/common/block_util.c", 
        f"{LIBLZMA_LOCATION}/common/vli_decoder.c", 
        f"{LIBLZMA_LOCATION}/common/filter_flags_decoder.c", 
        f"{LIBLZMA_LOCATION}/common/stream_flags_common.c", 
        f"{LIBLZMA_LOCATION}/common/vli_size.c", 
        f"{LIBLZMA_LOCATION}/check/crc32_small.c", 
        f"{LIBLZMA_LOCATION}/lzma/lzma2_decoder.c", 
        f"{LIBLZMA_LOCATION}/lzma/lzma_decoder.c", 
        f"{LIBLZMA_LOCATION}/lz/lz_decoder.c",
    ]

    if(config.target_system == System.WINDOWS):
        sources.append("installer/windows/include/windows_glue.c")
    elif(config.target_system == System.LINUX):
        sources.append("installer/linux/include/linux_glue.c")

    output_dir = f"{MODULES_DIR}lzma/{config.get_triple()}"
    os.makedirs(output_dir, exist_ok=True)

    build_shared_lzma(config, sources, output_dir)
    build_static_lzma(config, sources, output_dir)
    build_lzma_compressor(config, output_dir)

def build_shared_lzma(config: BuildConfig, sources: list[str], output_dir: str):
    compiler: Compiler = make_compiler(config)
    compiler.add_sources(sources)
    compiler.add_compilation_options(["-Iinstaller/shared/include", "-DHAVE_SMALL", "-DHAVE_DECODER_LZMA2", "-DHAVE_CHECK_CRC32", "-DHAVE_DECODERS", "-DHAVE__BOOL", "-DGLUE_SHARED"])
    compiler.optimize_for_size()
    compiler.disable_stdlib()
    link_flags = ["-s", "-lkernel32"]
    if(config.target_arch == Arch.Arm):
        link_flags.append("-lclang_rt.builtins-arm")
    compiler.compile_shared(f"{output_dir}/liblzma.so", link_flags)

def build_static_lzma(config: BuildConfig, sources: list[str], output_dir: str):
    compiler = make_compiler(config)
    compiler.add_sources(sources)
    compiler.add_compilation_options(["-Iinstaller/shared/include", "-DHAVE_SMALL", "-DHAVE_DECODER_LZMA2", "-DHAVE_CHECK_CRC32", "-DHAVE_DECODERS", "-DHAVE__BOOL"])
    compiler.optimize_for_size()
    compiler.compile_static(f"{output_dir}/liblzma.a")

def build_lzma_compressor(config: BuildConfig, output_dir: str):
    sources = [
        f"{LIBLZMA_LOCATION}/common/stream_encoder.c", 
        f"{LIBLZMA_LOCATION}/common/common.c", 
        f"{LIBLZMA_LOCATION}/common/index_hash.c",
        f"{LIBLZMA_LOCATION}/common/index.c", 
        f"{LIBLZMA_LOCATION}/common/index_encoder.c", 
        f"{LIBLZMA_LOCATION}/check/check.c", 
        f"{LIBLZMA_LOCATION}/common/filter_encoder.c", 
        f"{LIBLZMA_LOCATION}/common/block_encoder.c", 
        f"{LIBLZMA_LOCATION}/common/block_buffer_encoder.c", 
        f"{LIBLZMA_LOCATION}/common/block_header_encoder.c", 
        f"{LIBLZMA_LOCATION}/common/filter_common.c", 
        f"{LIBLZMA_LOCATION}/common/block_util.c", 
        f"{LIBLZMA_LOCATION}/common/vli_encoder.c",
        f"{LIBLZMA_LOCATION}/common/vli_decoder.c", 
        f"{LIBLZMA_LOCATION}/common/filter_flags_encoder.c",
        f"{LIBLZMA_LOCATION}/common/stream_flags_encoder.c", 
        f"{LIBLZMA_LOCATION}/common/stream_buffer_encoder.c", 
        f"{LIBLZMA_LOCATION}/common/easy_buffer_encoder.c", 
        f"{LIBLZMA_LOCATION}/common/easy_preset.c", 
        f"{LIBLZMA_LOCATION}/common/stream_flags_common.c", 
        f"{LIBLZMA_LOCATION}/common/vli_size.c", 
        f"{LIBLZMA_LOCATION}/check/crc32_fast.c", 
        f"{LIBLZMA_LOCATION}/lzma/fastpos_table.c", 
        f"{LIBLZMA_LOCATION}/lzma/lzma2_encoder.c", 
        f"{LIBLZMA_LOCATION}/lzma/lzma_encoder_optimum_fast.c", 
        f"{LIBLZMA_LOCATION}/lzma/lzma_encoder_optimum_normal.c", 
        f"{LIBLZMA_LOCATION}/lzma/lzma_encoder.c",
        f"{LIBLZMA_LOCATION}/lz/lz_encoder.c",
        f"{LIBLZMA_LOCATION}/rangecoder/price_table.c",
        f"{LIBLZMA_LOCATION}/lz/lz_encoder_mf.c",
        f"{LIBLZMA_LOCATION}/lzma/lzma_encoder_presets.c",
        "modules/lzma/lzma-compressor.c",
    ]
    compiler = make_compiler(config)
    compiler.add_sources(sources)
    compiler.add_compilation_options(["-Iinstaller/shared/include", "-DHAVE_ENCODER_LZMA2", "-DHAVE_ENCODER_LZMA1", "-DHAVE_CHECK_CRC32", "-DHAVE_ENCODERS", 
                                      "-DHAVE__BOOL", "-DCONDITIONAL_MATCH_FINDERS", "-DHAVE_MF_HC3", "-DHAVE_MF_HC4", "-DHAVE_MF_BT2", "-DHAVE_MF_BT3", "-DHAVE_MF_BT4"])
    compiler.compile_executable(f"{output_dir}/lzmacomp.exe")