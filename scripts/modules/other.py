import shutil

from ..build_config import BuildConfig
from ..globals import MODULES_OUTPUT_DIR

def build(config: BuildConfig):
    # Place modules that need to simply be copied.
    modules = [
        "quarkit_outputter",
        "quarkit_extraction",
        "quarkit_logging"
    ]
    
    for module in modules:
        output_dir = f"{MODULES_OUTPUT_DIR}/{module}"
        shutil.copytree(f"modules/{module}", f"{output_dir}", dirs_exist_ok=True)