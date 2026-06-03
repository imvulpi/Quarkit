import os
import shutil

from ..build_config import BuildConfig
from ..compiler import Compiler, make_compiler
from ..globals import MODULES_DIR

def build(config: BuildConfig):
    print("Building the brieflz module")
    sources = [
        "modules/brieflz/deps/blzpack.c", 
        "modules/brieflz/deps/brieflz.c", 
        "modules/brieflz/deps/parg.c", 
        "modules/brieflz/deps/depack.c", 
        "modules/brieflz/deps/depacks.c"
    ]
        
    output_dir = f"{MODULES_DIR}/brieflz/{config.get_triple()}"
    os.makedirs(output_dir, exist_ok=True)
    os.makedirs(f"{MODULES_DIR}/brieflz/deps", exist_ok=True)

    compiler: Compiler = make_compiler(config)
    compiler.add_sources(sources)
    compiler.optimize_for_size()
    compiler.compile_executable(f"{output_dir}/blzpack.exe")
    shutil.copyfile("modules/brieflz/module.json", f"{MODULES_DIR}/brieflz/module.json")
    shutil.copyfile("modules/brieflz/brieflz-extractor.c", f"{MODULES_DIR}/brieflz/brieflz-extractor.c")
    shutil.copyfile("modules/brieflz/deps/depack.c", f"{MODULES_DIR}/brieflz/deps/depack.c")
    shutil.copyfile("modules/brieflz/deps/brieflz.h", f"{MODULES_DIR}/brieflz/deps/brieflz.h")