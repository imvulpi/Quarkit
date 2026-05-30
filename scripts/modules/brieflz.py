import os

from ..build_config import BuildConfig
from ..compiler import Compiler, make_compiler
from ..globals import MODULES_DIR

def build(config: BuildConfig):
    print("Building the brieflz module")
    sources = [
        "modules/deps/brieflz/blzpack.c", 
        "modules/deps/brieflz/brieflz.c", 
        "modules/deps/brieflz/parg.c", 
        "modules/deps/brieflz/depack.c", 
        "modules/deps/brieflz/depacks.c"
    ]
        
    output_dir = f"{MODULES_DIR}/brieflz/{config.get_triple()}"
    os.makedirs(output_dir, exist_ok=True)

    compiler: Compiler = make_compiler(config)
    compiler.add_sources(sources)
    compiler.optimize_for_size()
    compiler.compile_executable(f"{output_dir}/blzpack.exe")