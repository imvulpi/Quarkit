import os
import shutil

from ..build_config import BuildConfig
from ..compiler import Compiler, make_compiler
from ..globals import MODULES_DIR

def build(config: BuildConfig):
    print("Building the brieflz_extractor module")
    sources = [
        "modules/brieflz_extractor/deps/blzpack.c", 
        "modules/brieflz_extractor/deps/brieflz.c", 
        "modules/brieflz_extractor/deps/parg.c", 
        "modules/brieflz_extractor/deps/depack.c", 
        "modules/brieflz_extractor/deps/depacks.c"
    ]
        
    output_dir = f"{MODULES_DIR}/brieflz_extractor/{config.get_triple()}"
    os.makedirs(output_dir, exist_ok=True)
    os.makedirs(f"{MODULES_DIR}/brieflz_extractor/deps", exist_ok=True)

    compiler: Compiler = make_compiler(config)
    compiler.add_sources(sources)
    compiler.optimize_for_size()
    compiler.compile_executable(f"{output_dir}/blzpack.exe")
    shutil.copyfile("modules/brieflz_extractor/module.json", f"{MODULES_DIR}/brieflz_extractor/module.json")
    shutil.copyfile("modules/brieflz_extractor/brieflz_extractor.c", f"{MODULES_DIR}/brieflz_extractor/brieflz_extractor.c")
    shutil.copyfile("modules/brieflz_extractor/deps/depack.c", f"{MODULES_DIR}/brieflz_extractor/deps/depack.c")
    shutil.copyfile("modules/brieflz_extractor/deps/brieflz.h", f"{MODULES_DIR}/brieflz_extractor/deps/brieflz.h")