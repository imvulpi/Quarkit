from dataclasses import dataclass
import glob
import os
from pathlib import Path
import shutil
from .build_enums import *

@dataclass
class BuildConfig:
    compiler_type: CompilerType
    target_system: System
    target_arch: Arch
    target_bitness: Bitness
    host_system: System

    def get_triple(self) -> str:
        return f"{self.target_system}-{self.target_arch}_{self.target_bitness}"
    
def get_source_files(pattern, excludes=[]):
    """Globs files matching the pattern and removes any files containing exclusion strings."""
    files = glob.glob(pattern, recursive=True)

    # Filter out files that match any of our exclusion substrings
    filtered_files = [
        f
        for f in files
        if not any(exclude in os.path.basename(f) for exclude in excludes)
    ]
    return filtered_files

def copy_files(pattern, output, excludes=[]):
    header_paths = get_source_files(f"{pattern}", excludes)
    for header_path in header_paths:
        output_path = Path(output, header_path)
        output_path.parent.mkdir(parents=True, exist_ok=True)
        shutil.copy2(header_path, output_path)