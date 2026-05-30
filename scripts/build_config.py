from dataclasses import dataclass
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