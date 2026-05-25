import os
from pathlib import Path
import subprocess

from ..build_enums import Arch, System
from ..build_config import BuildConfig
from ..globals import DIST_DIR

def build(config: BuildConfig):
    print("Building the CLI executable")
    cli_project = "cli/Quarkit.CLI/Quarkit.CLI.csproj"
    dotnet_rid = get_dotnet_rid(config.target_system, config.target_arch)
    if(dotnet_rid == ""): 
        print("Could not get dotnet RID.") 
        return
    
    output_dir = f"{DIST_DIR}/{config.target_system}_{config.target_arch}"
    os.makedirs(output_dir, exist_ok=True)

    cmd = (
        ["dotnet", "publish"] + [cli_project]
        + ["-c", "Release", "-p:PublishSingleFile=true"]
        + ["--self-contained", "false"]
        + ["-o", output_dir ]
    )

    subprocess.run(cmd, check=True)
    output_dir = Path(output_dir)

    for file_path in output_dir.glob("*.pdb"):
        file_path.unlink()


def get_dotnet_rid(target_system: System, target_arch: Arch) -> str:
    rid: str = ""
    if(target_system == System.LINUX): rid += "linux-"
    elif(target_system == System.WINDOWS): rid += "win-"
    elif(target_system == System.MACOS): rid += "osx-"
    else: return ""

    if(target_arch == Arch.X64): rid += "x64"
    elif(target_arch == Arch.X86): rid += "x86"
    elif(target_arch == Arch.ARM64): rid += "arm64"
    else: return ""
    return rid