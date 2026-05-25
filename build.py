#!/usr/bin/env python3

import os
import sys
from scripts.build_config import BuildConfig
from scripts.build_enums import Arch, CompilerType, System
from scripts.cli import cli
from scripts.modules import xz

def main():
    print("Initiating Quarkit Building...")
    os.makedirs("output", exist_ok=True)

    host_system: System = System.UNKNOWN
    if(sys.platform.startswith("win")): host_system = System.WINDOWS
    elif(sys.platform.startswith("linux")): host_system = System.LINUX
    configs = [BuildConfig(CompilerType.CLANG, System.WINDOWS, Arch.X86, host_system),
               BuildConfig(CompilerType.CLANG, System.WINDOWS, Arch.X64, host_system),
               BuildConfig(CompilerType.CLANG, System.WINDOWS, Arch.ARM64, host_system)]
    
    for config in configs:
        cli.build(config)
        xz.build(config)    

if __name__ == "__main__":
    main()