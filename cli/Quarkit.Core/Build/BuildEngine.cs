using Quarkit.Core.Processes;
using Quarkit.Core.Storage;
using Quarkit.Models.Manifest;
using Quarkit.Models.Manifest.Modules;
using System.Xml;

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

        public const string ESC_STRING = "\"\\\"";
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
            args.Add($"-I\"{Path.Combine(parameters.QuarkitRoot, Paths.GetInstallerIncludes(parameters.Target.System))}\"");
            args.Add($"-I\"{Path.Combine(parameters.QuarkitRoot, Paths.GetSharedInstallerIncludes())}\"");
            foreach (var resolved in parameters.ResolvedModules)
            {
                string includePath = Path.Combine(resolved.Module.ModuleDirectory, "include");
                args.Add($"-I\"{includePath}\"");
            }

            InjectCompilationOptions(args, parameters);
            if (parameters.PayloadPath != string.Empty && _fileSystem.FileExists(parameters.PayloadPath))
            {
                FileInfo info = new(parameters.PayloadPath);
                args.Add($"-DQUARKIT_PAYLOAD_SIZE={info.Length}"); // Payload size. (This doesn't change).
                args.Add($"-DQUARKIT_PAYLOAD_NAME={ESC_STRING}{parameters.PayloadName}{ESC_STRING}");
            }
            else
            {
                Console.WriteLine($"Payload is not a file! {parameters.PayloadPath}\n");
            }

            var dynamicModuleInits = new List<string>();
            var dynamicModuleDeInits = new List<string>();
            var dynamicModuleExterns = new List<string>();
            foreach (var resolved in parameters.ResolvedModules)
            {
                if (HasHook(resolved, resolved.Blueprint.HasInitHook, "init"))
                {
                    Console.WriteLine($"Found a init hook: {$"quarkit_{resolved.Module.Manifest.Id.Replace("-", "_")}_init();"}");
                    dynamicModuleInits.Add($"quarkit_{resolved.Module.Manifest.Id.Replace("-", "_")}_init();");
                    dynamicModuleExterns.Add($"extern void quarkit_{resolved.Module.Manifest.Id.Replace("-", "_")}_init(void);");
                }

                if (HasHook(resolved, resolved.Blueprint.HasDeInitHook, "deinit"))
                {
                    Console.WriteLine($"Found a deinit hook: {$"quarkit_{resolved.Module.Manifest.Id.Replace("-", "_")}_deinit();"}");
                    dynamicModuleDeInits.Add($"quarkit_{resolved.Module.Manifest.Id.Replace("-", "_")}_deinit();");
                    dynamicModuleExterns.Add($"extern void quarkit_{resolved.Module.Manifest.Id.Replace("-", "_")}_deinit(void);");
                }

                if (resolved.Blueprint.CompilerFlags?.Values != null)
                {
                    foreach (var flag in resolved.Blueprint.CompilerFlags.Values)
                    {
                        args.Add(flag);
                    }
                }
            }

            if (dynamicModuleInits.Count > 0)
            {
                string joinedInits = string.Join(" ", dynamicModuleInits);
                args.Add($"-DQUARKIT_MODULE_INITS=\"{joinedInits}\"");

                string joinedDeInits = string.Join(" ", dynamicModuleDeInits);
                args.Add($"-DQUARKIT_MODULE_DEINITS=\"{joinedDeInits}\"");

                string joinedExterns = string.Join(" ", dynamicModuleExterns);
                args.Add($"-DQUARKIT_MODULE_EXTERNS=\"{joinedExterns}\"");
            }

            foreach (var resolved in parameters.ResolvedModules)
            {
                foreach (var sourceFile in resolved.GetAbsoluteCSources())
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
            else
            {
                if (parameters.PayloadPath != string.Empty && _fileSystem.FileExists(parameters.PayloadPath))
                {
                    // Appends the payload to the executable
                    _fileSystem.AppendAllBytes(parameters.OutputPath, _fileSystem.ReadAllBytes(parameters.PayloadPath));
                }
            }
            Console.WriteLine();
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

        private bool HasHook(ResolvedModule resolvedMod, bool? hookOption, string hookName)
        {
            if (hookOption == null)
            {
                if (resolvedMod.Blueprint.CSources == null || resolvedMod.Blueprint.CSources?.Values?.Count <= 0) return false;

                // Very simple check inside the first file
                foreach (string line in _fileSystem.ReadLines(resolvedMod.GetAbsoluteCSources().First()))
                {
                    if(line.Contains("void") && line.Contains("quarkit") && line.Contains(hookName))
                    {
                        bool previousWasComment = false;
                        for (int i = 0; i < line.Length; i++)
                        {
                            var current = line[i];
                            
                            // Very simple check for comment.
                            if (previousWasComment && (current == '/' || current == '*')) return false;
                            
                            if (current == '/') previousWasComment = true;
                            else if (current == 'v') return true; // beginning of 'void'
                        }

                        return true;
                    }
                }

                return false;
            }
            else
            {
                if (hookOption == true) return true;
                if (hookOption == false) return false;
            }
            
            return false;
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
            args.Add($"-DQUARKIT_GENERATOR_VERSION={ESC_STRING}1.0.0-alpha.1{ESC_STRING}");
            args.Add($"-DQUARKIT_OS_{system}");
            args.Add($"-DQUARKIT_OS_NAME={ESC_STRING}{system}{ESC_STRING}");
            args.Add($"-DQUARKIT_ARCH_{architecture}");
            args.Add($"-DQUARKIT_ARCH_{architecture}_{bitness}");
            args.Add($"-DQUARKIT_ARCH_NAME={ESC_STRING}{architecture}{ESC_STRING}");
            args.Add($"-DQUARKIT_BITNESS_{bitness}");
            args.Add($"-DQUARKIT_BITNESS_NAME={ESC_STRING}{bitness}{ESC_STRING}");

            var options = parameters.ResolvedOptions;
            if (options.AdminRequired.HasValue && options.AdminRequired.Value) args.Add($"-DQUARKIT_REQUIRE_ADMIN");
            if (!string.IsNullOrEmpty(options.AppName)) args.Add($"-DQUARKIT_APP_NAME={ESC_STRING}{options.AppName}{ESC_STRING}");
            if (!string.IsNullOrEmpty(options.TargetPath)) args.Add($"-DQUARKIT_TARGET_PATH={ESC_STRING}{options.TargetPath}{ESC_STRING}");
            if (options.DesktopShortcut.HasValue && options.DesktopShortcut.Value)
                args.Add("-DQUARKIT_CREATE_DESKTOP_SHORTCUT");

            if (options.StartShortcut.HasValue && options.StartShortcut.Value)
                args.Add("-DQUARKIT_CREATE_START_SHORTCUT");

            if (!string.IsNullOrEmpty(options.ExecutableToLaunch))
            {
                args.Add("-DQUARKIT_HAS_LAUNCH_TARGET");
                args.Add($"-DQUARKIT_EXE_LAUNCH={ESC_STRING}{options.ExecutableToLaunch}{ESC_STRING}");
            }
        }
    }
}
