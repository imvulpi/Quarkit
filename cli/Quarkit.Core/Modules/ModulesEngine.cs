using Quarkit.Core.Manifest;
using Quarkit.Core.Processes;
using Quarkit.Core.Shorthand;
using Quarkit.Core.Storage;
using Quarkit.Models.Core.Target;
using Quarkit.Models.Manifest.Modules;
using System.Text.Json;

namespace Quarkit.Core.Modules
{
    public class ModulesEngine
    {
        /// <summary>
        /// The name of the module manifest file inside the module directory.
        /// </summary>
        public const string MODULE_FILENAME = "module.json";

        private readonly Dictionary<string, string> _globalModulesIndex = new(StringComparer.OrdinalIgnoreCase);
        private readonly string _qkRoot;
        private readonly IFileSystem _fileSystem;
        private readonly IProcessRunner _processRunner;

        public ModulesEngine(string qkRoot, IFileSystem? fileSystem = null, IProcessRunner? processRunner = null)
        {
            _qkRoot = qkRoot;
            _fileSystem = fileSystem ?? new PhysicalFileSystem();
            _processRunner = processRunner ?? new ProcessRunner();
            IndexGlobalModules();
        }

        /// <summary>
        /// Scans &lt;QK&gt;/modules/ and indexes core modules.
        /// </summary>
        private void IndexGlobalModules()
        {
            string globalModulesPath = Path.Combine(_qkRoot, "modules");
            if (!_fileSystem.DirectoryExists(globalModulesPath)) return;

            foreach (string dir in _fileSystem.GetDirectories(globalModulesPath))
            {
                string moduleName = Path.GetFileName(dir);
                _globalModulesIndex[moduleName] = dir;
            }
        }

        public ResolvedModule[] ResolveModules(string[] moduleIds, string manifestPath, TargetKey target, OverridesResolver resolver)
        {
            ResolvedModule[] declaredModules = new ResolvedModule[moduleIds.Length];
            for (int i = 0; i < moduleIds.Length; i++)
            {
                declaredModules[i] = ResolveModule(FindAndLoadModule(moduleIds[i], manifestPath), resolver, target);
            }

            return ResolveDependencies(declaredModules, manifestPath, resolver, target).ToArray();
        }

        /// <summary>
        /// Resolve a module string from the installation options into a LoadedModule.
        /// </summary>
        public LoadedModule FindAndLoadModule(string moduleDeclaration, string manifestPath)
        {
            string? resolvedPath = null;

            if (moduleDeclaration.StartsWith("<QK>/", StringComparison.OrdinalIgnoreCase)) // Explicit Core Shorthand (<QK>/module)
            {
                string moduleName = moduleDeclaration.Substring(5);
                if (_globalModulesIndex.TryGetValue(moduleName, out string? path)) resolvedPath = path;
                else
                {
                    resolvedPath = Path.Combine(_qkRoot, moduleName);
                }
            }
            else if (moduleDeclaration.StartsWith("./") || moduleDeclaration.StartsWith("../")) // Explicit relative path (./modules/custom)
            {
                resolvedPath = Path.GetFullPath(Path.Combine(manifestPath, moduleDeclaration));
            }
            else // Implicit check (Try local directory first, fallback to indexed global)
            {
                string localAttempt = Path.GetFullPath(Path.Combine(manifestPath, moduleDeclaration));
                if (_fileSystem.DirectoryExists(localAttempt))
                {
                    resolvedPath = localAttempt;
                }
                else if (_globalModulesIndex.TryGetValue(moduleDeclaration, out string? path))
                {
                    resolvedPath = path;
                }
            }

            if (resolvedPath == null || !_fileSystem.DirectoryExists(resolvedPath))
            {
                throw new DirectoryNotFoundException($"Quarkit module '{moduleDeclaration}' could not be resolved locally or globally.");
            }

            // Parse the module.json inside the target directory
            string manifestFilePath = Path.Combine(resolvedPath, MODULE_FILENAME);
            if (!_fileSystem.FileExists(manifestFilePath))
            {
                throw new FileNotFoundException($"Missing '{MODULE_FILENAME}' in resolved module directory: {resolvedPath}");
            }

            string jsonRaw = _fileSystem.ReadAllText(manifestFilePath);
            var manifest = JsonSerializer.Deserialize<ModuleManifest>(jsonRaw, new JsonSerializerOptions { PropertyNameCaseInsensitive = false })
                           ?? throw new InvalidDataException($"Failed to deserialize {MODULE_FILENAME} at {manifestFilePath}");

            return new LoadedModule
            {
                Manifest = manifest,
                ManifestPath = manifestFilePath,
                ModuleDirectory = resolvedPath
            };
        }

        public ResolvedModule ResolveModule(LoadedModule module, OverridesResolver resolver, TargetKey target)
        {
            if(module.Manifest.Overrides == null)
            {
                return new()
                {
                    Blueprint = module.Manifest.Default,
                    Module = module
                };
            }

            ModuleBlueprint blueprint = resolver.ResolveForTarget(module.Manifest.Default, [.. module.Manifest.Overrides], target, new(module.Manifest.Options ?? []));
            return new() { Blueprint = blueprint, Module = module };
        }

        public IEnumerable<ResolvedModule> ResolveDependencies(IEnumerable<ResolvedModule> modules, string manifestPath, OverridesResolver resolver, TargetKey target, 
            HashSet<string>? resolved = null)
        {
            List<ResolvedModule> finalList = [];
            resolved ??= [];

            foreach (ResolvedModule module in modules)
            {
                if (resolved.Contains(module.Module.Manifest.Id)) { continue; } // Skip because a dependency added it before and it would result in duplicate modules.
                if (module.Blueprint.Dependencies != null && module.Blueprint.Dependencies.Values != null)
                {
                    foreach (string dependency in module.Blueprint.Dependencies.Values)
                    {
                        if (resolved.Contains(dependency)) continue;
                        LoadedModule dependencyModule = FindAndLoadModule(dependency, manifestPath);
                        ResolvedModule resolvedDependencyModule = ResolveModule(dependencyModule, resolver, target);
                        finalList.AddRange(ResolveDependencies([resolvedDependencyModule], manifestPath, resolver, target, resolved));
                    }
                }
                finalList.Add(module);
            }
            return finalList;
        }

        /// <summary>
        /// Run the script commands and pipe stdout variables back to the ShorthandEngine.
        /// </summary>
        public void RunPreBuildCommands(LoadedModule module, ModuleBlueprint blueprint, ShorthandEngine shorthandEngine)
        {
            if (blueprint.PreBuildCommands == null) return;

            foreach (var command in blueprint.PreBuildCommands)
            {
                // Context tokens specific to this command execution context
                var contextTokens = new Dictionary<string, string>
                {
                    { "<ModuleDir>", module.ModuleDirectory },
                };

                string executable = executable = shorthandEngine.Expand(command.Executable, contextTokens);
                if (!command.Executable.StartsWith("<QK>")) // No root
                {
                    string possibleExecutable = Path.Combine(module.ModuleDirectory, executable);
                    if (_fileSystem.FileExists(possibleExecutable)) // Prefer local executables.
                    {
                        executable = possibleExecutable;
                    }
                    else if (command.Executable.StartsWith("./") || command.Executable.StartsWith("./")) // Forced a local executable
                    {
                        throw new InvalidDataException($"Module: {module.Manifest.Id} " +
                            $"tried to execute {command.Executable}, but there is no LOCAL {command.Executable}. Did you mean {command.Executable[2..]}?");
                    }
                }  // Otherwise it may be a executable registered in Paths of the system so we dont do anything.

                string arguments = command.Arguments == null ? string.Empty : shorthandEngine.Expand(command.Arguments, contextTokens);

                (int exitCode, string output, string error) = _processRunner.Execute(executable, arguments);

                command.SuccessCodes ??= [0]; // Default success code
                if (!command.SuccessCodes.Contains(exitCode))
                {
                    throw new Exception($"{module.Manifest.Id} module ran `{executable} {arguments}` and it failed with: {exitCode} exit code.\nStandardError output: {error}");
                }

                if (command.FailIfOutputContains != null && (output.Contains(command.FailIfOutputContains) || error.Contains(command.FailIfOutputContains)))
                {
                    throw new Exception($"{module.Manifest.Id} module ran `{executable} {arguments}`, and the output contains {command.FailIfOutputContains} which signifies failure.\nStandardError output: {error}");
                }

                if (command.FailOnStdErr != null && command.FailOnStdErr.Value && error != string.Empty)
                {
                    throw new Exception($"{module.Manifest.Id} module ran `{executable} {arguments}`, and the StandardError output is not empty which signifies failure.\nStandardError output: {error}");
                }

                if (command.ShorthandOverrides != null)
                {
                    foreach (var overrides in command.ShorthandOverrides)
                    {
                        shorthandEngine.SetToken(overrides.Key, shorthandEngine.Expand(overrides.Value));
                    }
                }

                if (command.CaptureVariables)
                {
                    // TODO: This could use a bit more checking:
                    string[] lines = output.Split(["\n", "\r\n"], StringSplitOptions.RemoveEmptyEntries);
                    for (int i = 0; i < lines.Length; i++)
                    {
                        string line = lines[i].Trim();
                        if (line.Contains('=')) // Very simple check
                        {
                            string[] kv = line.Split('=');
                            if (kv.Length >= 2)
                            {
                                shorthandEngine.SetToken(kv[0], kv[1]);
                            }
                        }
                    }
                }
            }
        }
    }
}
