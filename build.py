#!/usr/bin/env python3

import os
import shutil
import sys
from scripts.build_config import BuildConfig
from scripts.build_enums import Arch, CompilerType, System
from scripts.cli import cli
from scripts.globals import DIST_DIR, MODULES_DIR
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
    
    # First run to generate all the modules once
    for config in configs:
        xz.build(config)

    # Build cli for each config and copy the modules
    for config in configs:
        cli.build(config)
        dist_modules = f"{DIST_DIR}/{config.target_system}_{config.target_arch}/modules"
        shutil.copytree(MODULES_DIR, dist_modules, dirs_exist_ok=True)

    shutil.rmtree(MODULES_DIR)
    
if __name__ == "__main__":
    main()