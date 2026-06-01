using Quarkit.Core.Processes;
using Quarkit.Core.Storage;
using Quarkit.Models.Manifest;

namespace Quarkit.Core.Build
{
    public class BuildEngine
    {
        private readonly IFileSystem _fileSystem;
        private readonly IProcessRunner _processRunner;

        public BuildEngine(IFileSystem fileSystem, IProcessRunner processRunner)
        {
            _fileSystem = fileSystem;
            _processRunner = processRunner;
        }

        public void Build(BuildParameters parameters)
        {
            string? outputFolder = Path.GetDirectoryName(parameters.OutputPath);
            if (!string.IsNullOrEmpty(outputFolder) && !_fileSystem.DirectoryExists(outputFolder))
            {
                _fileSystem.CreateDirectory(outputFolder);
            }

            var args = new List<string>();

            ApplyTargetFlags(args, parameters);
            ApplyOptimizationArguments(args, parameters.CompilerType);

            args.Add("-o");
            args.Add($"\"{parameters.OutputPath}\"");
            args.Add($"-I\"{Path.Combine(Paths.GetInstallerIncludes(parameters.Target.System))}\"");
            foreach (var module in parameters.ActiveModules)
            {
                string includePath = Path.Combine(module.ModuleDirectory, "include");
                args.Add($"-I\"{includePath}\"");
            }

            InjectCompilationOptions(args, parameters);

            var dynamicModuleInits = new List<string>();
            foreach (var module in parameters.ActiveModules)
            {
                dynamicModuleInits.Add($"quarkit_{module.Manifest.Id.Replace("-", "_")}_init();"); // TODO: might not exists, checking or manifest option.
                if (module.Manifest.CompilerFlags != null)
                {
                    foreach (var flag in module.Manifest.CompilerFlags)
                    {
                        args.Add(flag);
                    }
                }
            }

            if (dynamicModuleInits.Count > 0)
            {
                string joinedInits = string.Join(" ", dynamicModuleInits);
                args.Add($"-DQUARKIT_MODULE_INITS=\"{joinedInits}\"");
            }

            // Core and module c source files.
            args.Add($"\"{Path.Combine(parameters.QuarkitRoot, Paths.GetInstallerDir(parameters.Target.System), "main.c")}\"");
            foreach (var module in parameters.ActiveModules)
            {
                foreach (var sourceFile in module.GetAbsoluteCSources())
                {
                    args.Add($"\"{sourceFile}\"");
                }
            }

            string formattedArgs = string.Join(" ", args);
            Console.WriteLine($"Running: {parameters.CompilerName} {formattedArgs}");
            var result = _processRunner.Execute(parameters.CompilerName, formattedArgs);

            if (result.ExitCode != 0)
            {
                throw new Exception($"Compilation failed via toolchain '{parameters.CompilerName}'.\nError Details: {result.Error}");
            }
        }

        private void ApplyTargetFlags(List<string> args, BuildParameters parameters)
        {
            if (parameters.CompilerType == CompilerType.Gcc)
            {
                if (parameters.Target.System == OSSystem.Windows)
                {
                    parameters.CompilerName = GetGccTargetWindows(parameters.Target.Arch, parameters.Target.Bit);
                }
                else if (parameters.Target.System == OSSystem.Linux)
                {
                    parameters.CompilerName = GetGccTargetLinux(parameters.Target.Arch, parameters.Target.Bit);
                }
            }
            else
            {
                if (parameters.Target.System == OSSystem.Windows)
                {
                    args.Add("-target");
                    args.Add(GetClangTargetWindows(parameters.Target.Arch, parameters.Target.Bit));
                }
                else if (parameters.Target.System == OSSystem.Linux)
                {
                    args.Add("-target");
                    args.Add(GetClangTargetLinux(parameters.Target.Arch, parameters.Target.Bit));
                }
            }
        }


        private string GetGccTargetWindows(Architecture targetArch, Bitness targetBitness) {
            if (targetArch == Architecture.x86 && targetBitness == Bitness.x64) return "x86_64-w64-mingw32-gcc";
            else if (targetArch == Architecture.x86 && targetBitness == Bitness.x32) return "i686-w64-mingw32-gcc";
            else if (targetArch == Architecture.Arm && targetBitness == Bitness.x64) return "aarch64-w64-mingw32-gcc";
            else if (targetArch == Architecture.Arm && targetBitness == Bitness.x32) return "armv7-w64-mingw32-gcc";
            return string.Empty;
        }

        private string GetGccTargetLinux(Architecture targetArch, Bitness targetBitness) {
            if (targetArch == Architecture.x86 && targetBitness == Bitness.x64) return "x86_64-linux-gnu-gcc";
            else if (targetArch == Architecture.x86 && targetBitness == Bitness.x32) return "i686-linux-gnu-gcc";
            else if (targetArch == Architecture.Arm && targetBitness == Bitness.x64) return "aarch64-linux-gnu-gcc";
            else if (targetArch == Architecture.Arm && targetBitness == Bitness.x32) return "arm-linux-gnueabihf-gcc";
            return string.Empty;
        }

        private string GetClangTargetWindows(Architecture targetArch, Bitness targetBitness) {
            if (targetArch == Architecture.x86 && targetBitness == Bitness.x64) return "x86_64-w64-windows-gnu";
            else if (targetArch == Architecture.x86 && targetBitness == Bitness.x32) return "i686-w64-windows-gnu";
            else if (targetArch == Architecture.Arm && targetBitness == Bitness.x64) return "aarch64-w64-windows-gnu";
            else if (targetArch == Architecture.Arm && targetBitness == Bitness.x32) return "armv7-w64-windows-gnu";
            return string.Empty;
        }

        private string GetClangTargetLinux(Architecture targetArch, Bitness targetBitness) {
            if (targetArch == Architecture.x86 && targetBitness == Bitness.x64) return "x86_64-linux-gnu";
            else if (targetArch == Architecture.x86 && targetBitness == Bitness.x32) return "i686-linux-gnu";
            else if (targetArch == Architecture.Arm && targetBitness == Bitness.x64) return "aarch64-linux-gnu";
            else if (targetArch == Architecture.Arm && targetBitness == Bitness.x32) return "arm-linux-gnueabihf";
            return string.Empty;
        }

        private void ApplyOptimizationArguments(List<string> args, CompilerType compilerType)
        {
            if (compilerType == CompilerType.Custom) return;

            args.Add("-Oz");
            args.Add("-nostdlib");
            args.Add("-nostdlibinc");
            args.Add("-fuse-ld=lld");
            args.Add("-lkernel32");
        }

        private void InjectCompilationOptions(List<string> args, BuildParameters parameters)
        {
            string system = parameters.Target.System.AsString().ToUpper();
            string architecture = parameters.Target.Arch.AsString().ToUpper();
            string bitness = parameters.Target.Bit.AsString();

            args.Add($"-DQUARKIT_BUILD_TIMESTAMP={DateTimeOffset.UtcNow.ToUnixTimeSeconds()}");
            args.Add($"-DQUARKIT_GENERATOR_VERSION=\"1.0.0-alpha.1\"");
            args.Add($"-DQUARKIT_OS_{system}");
            args.Add($"-DQUARKIT_OS_NAME=\"{system}\"");
            args.Add($"-DQUARKIT_ARCH_{architecture}");
            args.Add($"-DQUARKIT_ARCH_{architecture}_{bitness}");
            args.Add($"-DQUARKIT_ARCH_NAME=\"{architecture}\"");
            args.Add($"-DQUARKIT_BITNESS_{bitness}");
            args.Add($"-DQUARKIT_BITNESS_NAME=\"{bitness}\"");

            var options = parameters.ResolvedOptions;
            if (options.AdminRequired.HasValue && options.AdminRequired.Value) args.Add($"-DQUARKIT_REQUIRE_ADMIN");
            if (!string.IsNullOrEmpty(options.AppName)) args.Add($"-DQUARKIT_APP_NAME=\"{options.AppName}\"");
            if (!string.IsNullOrEmpty(options.TargetPath)) args.Add($"-DQUARKIT_TARGET_PATH=\"{options.TargetPath}\"");
            if (options.DesktopShortcut.HasValue && options.DesktopShortcut.Value)
                args.Add("-DQUARKIT_CREATE_DESKTOP_SHORTCUT");

            if (options.StartShortcut.HasValue && options.StartShortcut.Value)
                args.Add("-DQUARKIT_CREATE_START_SHORTCUT");

            if (!string.IsNullOrEmpty(options.ExecutableToLaunch))
            {
                args.Add("-DQUARKIT_HAS_LAUNCH_TARGET");
                args.Add($"-DQUARKIT_EXE_LAUNCH=\"{options.ExecutableToLaunch}\"");
            }
        }
    }
}
