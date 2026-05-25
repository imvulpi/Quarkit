from enum import Enum, auto

class System(Enum):
    UNKNOWN = auto()
    WINDOWS = auto()
    LINUX = auto()
    MACOS = auto()
    
    def __str__(self):
        return self.name.lower()

class Arch(Enum):
    X64 = auto()
    ARM64 = auto()
    X86 = auto()  # 32 Bit.
    
    def __str__(self):
        return self.name.lower()

class CompilerType(Enum):
    GCC = auto()
    CLANG = auto()