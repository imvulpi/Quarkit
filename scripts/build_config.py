from dataclasses import dataclass
from .build_enums import *

@dataclass
class BuildConfig:
    compiler_type: CompilerType
    target_system: System
    target_arch: Arch
    host_system: System