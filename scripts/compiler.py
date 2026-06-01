from abc import ABC, abstractmethod
from concurrent.futures import ThreadPoolExecutor
import os
import subprocess

from .build_enums import Arch, Bitness, CompilerType, System
from .build_config import BuildConfig
from .globals import RUN_PARALLEL

class Compiler(ABC):
    def __init__(self, config: BuildConfig, executable: str):
        self.config = config
        self.sources = []
        self.includes = []
        self.flags = []
        self.compilation_options = []
        self.executable = executable

    def add_source(self, path: str):
        self.sources.append(path)
        return self  # Returning self allows method chaining!

    def add_sources(self, paths: list[str] | str):
        if isinstance(paths, str):
            paths = [paths]
        self.sources.extend(paths)

    def add_include(self, path: str):
        self.includes.append(f"-I{path}")
        return self

    def add_compilation_options(self, options: list[str] | str):
        if isinstance(options, str):
            options = [options]
        self.compilation_options.extend(options)

    @abstractmethod
    def disable_stdlib(self):
        """Disables the stdlib and stdlibinc"""
        pass

    @abstractmethod
    def optimize_for_size(self):
        """Apply extreme size optimization flags."""
        pass

    @abstractmethod
    def compile_static(self, output_path: str):
        """Execute compilation to a static (.a / .lib) library."""
        pass

    @abstractmethod
    def compile_shared(self, output_path: str):
        """Execute compilation to a shared (.so / .dll) library."""
        pass
    
    @abstractmethod
    def compile_executable(self, output_path: str):
        """Execute compilation to a executable (.exe / linux executable)."""
        pass

class GccCompiler(Compiler):
    def disable_stdlib(self):
        self.flags.extend([
            "-nostdlib",
            "-nostdlibinc",
        ])
        return self

    def optimize_for_size(self):
        self.flags.extend([
            "-Os",                  # Optimize for size
            "-s",                   # Strip symbols entirely
            "-ffunction-sections",  # Isolate functions so dead code can be removed
            "-fdata-sections",      # Isolate data sections
            "-fno-ident",           # Drop compiler identification strings
            "-fno-asynchronous-unwind-tables" # Drop stack unwinding tables
        ])
        if self.config.system == "windows":
            self.flags.append("-mwindows") # Prevent a console window flashing
        return self

    def compile_static(self, output_path: str):
        objects = []
        for src in self.sources:
            obj = src.replace(".c", ".o")
            cmd = [self.config.c_compiler] + self.flags + self.includes + ["-c", src, "-o", obj]
            subprocess.run(cmd, check=True)
            objects.append(obj)

class ClangCompiler(Compiler):
    def __init__(self, config: BuildConfig, executable: str = "clang"):
        super().__init__(config, executable)

    def disable_stdlib(self):
        self.flags.extend([
            "-nostdlib",
            "-nostdlibinc",
        ])
        return self

    def optimize_for_size(self):
        self.flags.extend([
            "-Oz",
            "-flto",
            "-fno-exceptions",
            "-ffunction-sections",
            "-fdata-sections"
        ])
        return self

    def compile_static(self, output_path):
        return super().compile_static(output_path)

    def compile_shared(self, output_path: str, link_flags: list[str] = []):
        config_folder = f"{self.config.get_triple()}"
        base_build_dir = os.path.join("build", config_folder)

        def compile_single(src):
            obj = src.replace(".c", ".o")
            rel_src = os.path.relpath(src)
            rel_obj = os.path.splitext(rel_src)[0] + ".o"
            obj = os.path.join(base_build_dir, rel_obj)
            os.makedirs(os.path.dirname(obj), exist_ok=True)

            cmd = (
                [self.executable]
                + self.flags
                + self.includes
                + ["-c", src, "-o", obj]
                + self.compilation_options
            )

            # Only compile newer:
            if not os.path.exists(obj) or os.path.getmtime(src) > os.path.getmtime(
                obj
            ): subprocess.run(cmd, check=True)
            
            return obj

        print("  Starting parallel compilation...")
        
        # Compile in parallel
        if(RUN_PARALLEL):
            with ThreadPoolExecutor(max_workers=os.cpu_count()) as executor:
                self.objects = list(executor.map(compile_single, self.sources))
        else:
            self.objects = list()
            for source in self.sources:
                self.objects.append(compile_single(source))

        print(f"  Compiled {len(self.sources)} files successfully.")
        
        link_cmd = (
            [self.executable, "-shared"]
            + link_flags
            + self.flags
            + self.objects
            + ["-o", output_path]
        )
        subprocess.run(link_cmd, check=True)
        print(f"  Successfully created shared library: {output_path}")
    
    def compile_static(self, output_path: str):
        config_folder = f"{self.config.get_triple()}"
        base_build_dir = os.path.join("build", config_folder)
        
        def compile_single(src):
            obj = src.replace(".c", ".o")
            rel_src = os.path.relpath(src)
            rel_obj = os.path.splitext(rel_src)[0] + ".o"
            obj = os.path.join(base_build_dir, rel_obj)
            os.makedirs(os.path.dirname(obj), exist_ok=True)
            
            cmd = (
                [self.executable]
                + self.flags
                + self.includes
                + ["-c", src, "-o", obj]
                + self.compilation_options
            )

            # Only compile newer:
            if not os.path.exists(obj) or os.path.getmtime(src) > os.path.getmtime(
                obj
            ): subprocess.run(cmd, check=True)
            
            return obj

        print("  Starting parallel compilation...")

        if(RUN_PARALLEL):
            with ThreadPoolExecutor(max_workers=os.cpu_count()) as executor:
                self.objects = list(executor.map(compile_single, self.sources))
        else:
            self.objects = list()
            for source in self.sources:
                self.objects.append(compile_single(source))

        print(f"  Compiled {len(self.objects)} files successfully.")

        link_cmd = (
            ["llvm-ar", "rcs", output_path] + self.objects
        )

        subprocess.run(link_cmd, check=True)

        print(f"  Successfully created static library: {output_path}")
    
    def compile_executable(self, output_path: str, link_flags: list[str] = []):
        config_folder = f"{self.config.get_triple()}"
        base_build_dir = os.path.join("build", config_folder)
        
        def compile_single(src):
            obj = src.replace(".c", ".o")
            rel_src = os.path.relpath(src)
            rel_obj = os.path.splitext(rel_src)[0] + ".o"
            obj = os.path.join(base_build_dir, rel_obj)
            os.makedirs(os.path.dirname(obj), exist_ok=True)
            
            cmd = (
                [self.executable]
                + self.flags
                + self.includes
                + ["-c", src, "-o", obj]
                + self.compilation_options
            )

            # Only compile newer:
            if not os.path.exists(obj) or os.path.getmtime(src) > os.path.getmtime(
                obj
            ): subprocess.run(cmd, check=True)
            
            return obj

        print("  Starting parallel compilation...")

        if(RUN_PARALLEL):
            with ThreadPoolExecutor(max_workers=os.cpu_count()) as executor:
                self.objects = list(executor.map(compile_single, self.sources))
        else:
            self.objects = list()
            for source in self.sources:
                self.objects.append(compile_single(source))

        print(f"  Compiled {len(self.objects)} files successfully.")

        link_cmd = (
            [self.executable]
            + link_flags
            + self.flags 
            + self.includes 
            + self.objects
            + ["-o", output_path] 
        )

        subprocess.run(link_cmd, check=True)

        print(f"  Successfully created executable: {output_path}")
    

def make_compiler(config: BuildConfig) -> Compiler:    
    compiler: Compiler
    if config.target_system == System.WINDOWS:
        if config.compiler_type == CompilerType.GCC:
            gcc_name = get_gcc_name_windows(config.target_arch)
            compiler = GccCompiler(config, gcc_name)
        else:
            clang_target = get_clang_target_windows(config.target_arch, config.target_bitness)
            compiler = ClangCompiler(config)
            compiler.flags.append("-target")
            compiler.flags.append(clang_target)
    elif config.target_system == System.LINUX:
        if config.compiler_type == CompilerType.GCC:
            gcc_name = get_gcc_name_linux(config.target_arch)
            compiler = GccCompiler(config, gcc_name)
        else:
            clang_target = get_clang_target_linux(config.target_arch, config.target_bitness)
            compiler = ClangCompiler(config)
            compiler.flags.append("-target")
            compiler.flags.append(clang_target)
    
    return compiler    

def get_gcc_name_windows(target_arch: Arch, target_bitness: Bitness):
    if target_arch == Arch.x86 and target_bitness == Bitness.x64:
        return "x86_64-w64-mingw32-gcc"
    elif target_arch == Arch.x86 and target_bitness == Bitness.x32:
        return "i686-w64-mingw32-gcc"
    elif target_arch == Arch.Arm and target_bitness == Bitness.x64:
        return "aarch64-w64-mingw32-gcc"
    elif target_arch == Arch.Arm and target_bitness == Bitness.x32:
        return "armv7-w64-mingw32-gcc" 
    
def get_gcc_name_linux(target_arch: Arch, target_bitness: Bitness):
    if target_arch == Arch.x86 and target_bitness == Bitness.x64:
        return "x86_64-linux-gnu-gcc"
    elif target_arch == Arch.x86 and target_bitness == Bitness.x32:
        return "i686-linux-gnu-gcc"
    elif target_arch == Arch.Arm and target_bitness == Bitness.x64:
        return "aarch64-linux-gnu-gcc"
    elif target_arch == Arch.Arm and target_bitness == Bitness.x32:
        return "arm-linux-gnueabihf-gcc"

def get_clang_target_windows(target_arch: Arch, target_bitness: Bitness):
    if target_arch == Arch.x86 and target_bitness == Bitness.x64:
        return "x86_64-w64-windows-gnu"
    elif target_arch == Arch.x86 and target_bitness == Bitness.x32:
        return "i686-w64-windows-gnu"
    elif target_arch == Arch.Arm and target_bitness == Bitness.x64:
        return "aarch64-w64-windows-gnu"
    elif target_arch == Arch.Arm and target_bitness == Bitness.x32:
        return "armv7-w64-windows-gnu"        
        
def get_clang_target_linux(target_arch: Arch, target_bitness: Bitness):
    if target_arch == Arch.x86 and target_bitness == Bitness.x64:
        return "x86_64-linux-gnu"
    elif target_arch == Arch.x86 and target_bitness == Bitness.x32:
        return "i686-linux-gnu"
    elif target_arch == Arch.Arm and target_bitness == Bitness.x64:
        return "aarch64-linux-gnu"
    elif target_arch == Arch.Arm and target_bitness == Bitness.x32:
        return "arm-linux-gnueabihf"