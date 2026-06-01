from dataclasses import dataclass
import glob
import os
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
    files = glob.glob(pattern)

    # Filter out files that match any of our exclusion substrings
    filtered_files = [
        f
        for f in files
        if not any(exclude in os.path.basename(f) for exclude in excludes)
    ]
    return filtered_files
