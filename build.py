import glob
import os
import subprocess
import shutil
import sys
import argparse

# --- CONFIGURATION CONSTANTS ---
SCRIPT_NAME = os.path.basename(__file__)
PROJECT_ROOT = os.path.dirname(os.path.abspath(__file__))
DIST_DIR = os.path.join(PROJECT_ROOT, "dist")
BUILD_ROOT = os.path.join(PROJECT_ROOT, "build")
CLI_EXECUTABLE_NAMES = ("quarkit", "quarkit.exe")
LIBRARY_EXTENSIONS = (".dll", ".so", ".dylib")
LIBRARY_KEYWORD = "qkit_builder"
OBJECT_EXTENSIONS = (".o", ".obj")
OBJECT_KEYWORD = "qkit_runtime"
TARGETS = [
    {"name": "windows-x64", "toolchain": "toolchains/x86_64-w64-mingw32.cmake"},
    {"name": "linux-x64", "toolchain": "toolchains/x86_64-linux-gnu.cmake"},
    {"name": "macos-x64", "toolchain": "toolchains/x86_64-apple-darwin.cmake"},
    {"name": "macos-arm64", "toolchain": "toolchains/aarch64-apple-darwin.cmake"},
]

# --- SUPPORTED ENV VARIABLES --- 
# APPLE_SDK_PATH - Path to a custom macOS SDK directory when cross-compiling
# LINUX_SYSROOT  - Path to linux sysroot when cross-compiling

def is_wsl():
    """Checks /proc/version to detect whether the system is a WSL subsystem"""
    try:
        with open("/proc/version", "r") as f:
            version_info = f.read().lower()
            return "microsoft" in version_info or "wsl" in version_info
    except FileNotFoundError:
        return False

def install_winget_package(package_id, human_name):
    try:
        subprocess.run(["winget", "install", "--id", package_id, "-e", "--accept-source-agreements", "--accept-package-agreements"], check=True)
    except subprocess.CalledProcessError:
        print(f"Warning: Failed to install {human_name} (id: {package_id}) via winget.")

def ensure_host_dependencies():
    """Detects missing compiler/toolchain dependencies across Windows, Linux, and macOS and prompts to auto-install."""

    missing = []
    if not shutil.which("cmake"): missing.append("cmake")        
    if not any(shutil.which(c) for c in ["clang", "gcc", "cc", "c++", "cl"]): missing.append("compiler (clang/gcc/msvc)")
    if sys.platform != "win32" and not shutil.which("x86_64-w64-mingw32-gcc") and not shutil.which("lld"): missing.append("mingw/lld")
    if not missing:
        return

    print(f"\n[Dependency Check] Missing recommended build dependencies: {', '.join(missing)}")
    print("Note: If you are using a unusual compiler, you can skip this.")
    
    choice = input("Would you like to automatically install the standard toolchain? [y/N]: ").strip().lower()
    if choice != 'y':
        print("Skipping automatic installation. Proceeding with existing environment...")
        return

    if sys.platform == "win32":
        if not shutil.which("winget"):
            print("Error: 'winget' package manager is not available on this Windows system.")
            print("Please manually install CMake and an appropriate compiler (e.g., LLVM Clang or MSVC).")
            return
        
        print("Auto-installing missing packages via Windows Package Manager (winget)...")
        if "cmake" in missing: install_winget_package("Kitware.CMake", "CMake")
        if any("compiler" in m for m in missing): install_winget_package("LLVM.LLVM", "Clang Compiler")
                
        print("Installation commands completed. Note: You may need to restart your terminal/script to refresh the PATH.")
    elif sys.platform.startswith("linux"):
        if shutil.which("apt-get"): # Debian/Ubuntu
            cmd = ["sudo", "apt-get", "update", "&&", "sudo", "apt-get", "install", "-y", 
                   "build-essential", "cmake", "clang", "lld", "mingw-w64"]
        elif shutil.which("dnf"): # Fedora/RHEL
            cmd = ["sudo", "dnf", "install", "-y", "cmake", "clang", "lld", "mingw64-gcc"]
        elif shutil.which("pacman"): # Arch Linux
            cmd = ["sudo", "pacman", "-S", "--noconfirm", "cmake", "clang", "lld", "mingw-w64-gcc"]
        elif shutil.which("apk"): # Alpine Linux
            cmd = ["sudo", "apk", "add", "cmake", "clang", "lld", "mingw-w64-gcc"]
        else:
            print("Unknown Linux distribution. Please install cmake, clang/gcc, and mingw-w64 manually.")
            return

        print(f"Auto-installing missing packages via package manager...")
        subprocess.run(" ".join(cmd), shell=True, check=True)
    elif sys.platform == "darwin": # macOS (Homebrew)
        if shutil.which("brew"):
            print("Auto-installing missing packages via Homebrew...")
            subprocess.run(["brew", "install", "cmake", "llvm", "mingw-w64"], check=True)
        else:
            print("Homebrew not found. Please install Homebrew or manually configure cmake and compilers.")

def convert_win_path_to_wsl(win_path):
    drive, tail = os.path.splitdrive(win_path)
    drive_letter = drive.replace(":", "").lower()
    tail = tail.replace("\\", "/")
    return f"/mnt/{drive_letter}{tail}"

def is_wsl_available():
    """Checks if the host system is windows and whether WSL subsystem is available"""
    if sys.platform != "win32" or shutil.which("wsl") is None:
        return False
    try:
        res = subprocess.run(["wsl", "echo", "1"], capture_output=True, text=True)
        return res.returncode == 0
    except Exception:
        return False

def is_docker_available():
    """Checks if the docker tool is installed on the system"""
    if shutil.which("docker") is None:
        return False
    try:
        res = subprocess.run(["docker", "info"], capture_output=True, text=True)
        return res.returncode == 0
    except Exception:
        return False

def has_macos_sdk():
    """Checks if a macOS SDK is available via env, local folder, or native xcrun."""
    if sys.platform == "darwin":
        return True
        
    if "APPLE_SDK_PATH" in os.environ and os.path.exists(os.environ["APPLE_SDK_PATH"]):
        return True
    
    local_sdk = os.path.join(PROJECT_ROOT, ".sdks", "MacOSX.sdk")
    if os.path.exists(local_sdk):
        return True

    return False

def has_linux_sysroot():
    """Checks if a linux sysroot is available via env"""
    if sys.platform.startswith("linux"):
        return True
        
    if "LINUX_SYSROOT" in os.environ and os.path.exists(os.environ["LINUX_SYSROOT"]):
        return True

    return False

def copy_static_library(build_dir, dist_dir, lib_name):
    """Locates and copies a compiled static library (.a or .lib) from the CMake build."""
    target_sub_dir = os.path.join(dist_dir, "runtime_libs")
    os.makedirs(target_sub_dir, exist_ok=True)
    
    # Platform-agnostic patterns
    patterns = [
        os.path.join(build_dir, "**", f"lib{lib_name}.a"),
        os.path.join(build_dir, "**", f"lib{lib_name}_static.a"),
        os.path.join(build_dir, "**", f"{lib_name}.lib"),
        os.path.join(build_dir, "**", f"lib{lib_name}_static.lib"),
        os.path.join(build_dir, "**", f"{lib_name}_static.lib"),
    ]
    
    found_lib = None
    for pattern in patterns:
        matches = glob.glob(pattern, recursive=True)
        if matches:
            found_lib = matches[0]
            break
            
    if found_lib:
        dest = os.path.join(target_sub_dir, os.path.basename(found_lib))
        shutil.copy2(found_lib, dest)
        print(f"[+] Copied '{lib_name}' static library to: {dest}")
        return dest
    else:
        print(f"[-] Warning: Static library for '{lib_name}' not found in the build.")
        return None

def build_target_native(target):
    target_name = target["name"]
    toolchain_file = os.path.join(PROJECT_ROOT, target["toolchain"])
    build_dir = os.path.join(BUILD_ROOT, target_name)
    output_dir = os.path.join(DIST_DIR, target_name)

    if "macos" in target_name and not has_macos_sdk():
        print(f"\n========================================")
        print(f"Skipping target: {target_name}")
        print(f"Reason: No macOS SDK found (set APPLE_SDK_PATH or place SDK in .sdks/MacOSX.sdk).")
        print(f"========================================")
        return False

    if "linux" in target_name and not has_linux_sysroot():
        print(f"\n========================================")
        print(f"Skipping target: {target_name}")
        print(f"Reason: No Linux Sysroot found (set LINUX_SYSROOT).")
        print(f"========================================")
        return False
    
    if(not is_wsl()): # Skip for WSL so it doesn't say Native and WSL.
        print(f"\n========================================")
        print(f"Building target NATIVELY: {target_name}")
        print(f"========================================")

    if not os.path.exists(toolchain_file):
        print(f"Warning: Toolchain file not found at {toolchain_file}. Skipping.")
        return False

    cmake_config_args = [
        "cmake",
        "-B", build_dir,
        "-S", PROJECT_ROOT,
        f"-DCMAKE_TOOLCHAIN_FILE={toolchain_file}",
        "-DCMAKE_BUILD_TYPE=Release"
    ]

    if "LINUX_SYSROOT" in os.environ and "linux" in target_name:
        cmake_config_args.append(f"-DCMAKE_SYSROOT={os.environ['LINUX_SYSROOT']}")
    if "APPLE_SDK_PATH" in os.environ and "macos" in target_name:
        cmake_config_args.append(f"-DAPPLE_SDK_PATH={os.environ['APPLE_SDK_PATH']}")

    try:
        subprocess.run(cmake_config_args, check=True)
        subprocess.run(["cmake", "--build", build_dir, "--config", "Release"], check=True)
    except subprocess.CalledProcessError as e:
        print(f"Build failed for {target_name}: {e}")
        return False

    os.makedirs(output_dir, exist_ok=True)
    for root, _, files in os.walk(build_dir):
        for file in files:
            file_lower = file.lower()
            src_path = os.path.join(root, file)
            
            if file_lower in CLI_EXECUTABLE_NAMES:
                shutil.copy(src_path, output_dir)
            elif file_lower.endswith(LIBRARY_EXTENSIONS) and LIBRARY_KEYWORD in file_lower:
                shutil.copy(src_path, output_dir)
            elif file_lower.endswith(OBJECT_EXTENSIONS) and OBJECT_KEYWORD in root.lower():
                obj_dir = os.path.join(output_dir, "runtime_objects")
                os.makedirs(obj_dir, exist_ok=True)
                shutil.copy(src_path, obj_dir)

    copy_static_library(build_dir, output_dir, "zstd")

    print(f"Successfully built: {target_name}")
    return True

def build_target_wsl(target_name):
    print(f"\n========================================")
    print(f"Building target via WSL: {target_name}")
    print(f"========================================")

    wsl_project_root = convert_win_path_to_wsl(PROJECT_ROOT)
    wsl_cmd = [
        "wsl", "bash", "-c",
        f"cd '{wsl_project_root}' && python3 {SCRIPT_NAME} --target {target_name} --force-native"
    ]
    try:
        subprocess.run(wsl_cmd, check=True)
        return True
    except subprocess.CalledProcessError as e:
        print(f"WSL build failed for {target_name}: {e}")
        return False

def build_target_docker(target_name):
    print(f"\n========================================")
    print(f"Building target via Docker: {target_name}")
    print(f"========================================")

    docker_cmd = [
        "docker", "run", "--rm",
        "-v", f"{PROJECT_ROOT}:/src",
        "-w", "/src",
        "alpine:latest",
        "sh", "-c", f"apk add --no-cache cmake clang make musl-dev python3 && python3 {SCRIPT_NAME} --target {target_name} --force-native"
    ]
    try:
        subprocess.run(docker_cmd, check=True)
        return True
    except subprocess.CalledProcessError as e:
        print(f"Docker build failed for {target_name}: {e}")
        return False

def main():
    print("Quarkit Build Script - Helpful distribution")
    parser = argparse.ArgumentParser(description="Multi-platform build runner.")
    parser.add_argument("--wsl", action="store_true", help="Force WSL mode for Linux targets")
    parser.add_argument("--docker", action="store_true", help="Force Docker mode for Linux targets")
    parser.add_argument("--force-native", action="store_true", help="Force native mode for the target")
    parser.add_argument("--target", type=str, help="Specify a single target to build")
    args = parser.parse_args()
    
    ensure_host_dependencies()

    targets_to_build = TARGETS
    if args.target:
        targets_to_build = [t for t in TARGETS if t["name"] == args.target]

    for target in targets_to_build:
        target_name = target["name"]

        if "linux" in target_name and sys.platform == "win32" and not args.force_native:
            if args.wsl or (not args.docker and is_wsl_available()):
                build_target_wsl(target_name)
            elif args.docker or is_docker_available():
                build_target_docker(target_name)
            else:
                build_target_native(target)
        else:
            build_target_native(target)

if __name__ == "__main__":
    main()