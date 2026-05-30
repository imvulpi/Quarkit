from enum import Enum, auto

class System(Enum):
    UNKNOWN = auto()
    WINDOWS = auto()
    LINUX = auto()
    MACOS = auto()
    
    def __str__(self):
        return self.name.lower()

class Arch(Enum):
    x86 = auto()
    Arm = auto()
    RiscV = auto()

    def __str__(self):
        return self.name.lower()
    
class Bitness(Enum):
    x32 = auto()
    x64 = auto()

    def __str__(self):
        return self.name.removeprefix('x')

class CompilerType(Enum):
    GCC = auto()
    CLANG = auto()