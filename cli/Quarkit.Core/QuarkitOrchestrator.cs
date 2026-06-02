using Quarkit.Core.Build;
using Quarkit.Core.Discovery;
using Quarkit.Core.Installation;
using Quarkit.Core.Modules;
using Quarkit.Core.Processes;
using Quarkit.Core.Shorthand;
using Quarkit.Core.Storage;
using Quarkit.Models.Manifest;
using Quarkit.Models.Manifest.Modules;

namespace Quarkit.Core
{
    public class QuarkitOrchestrator
    {
        public InstallationManifestEngine ManifestEngine { get; set; } = new();

        public const string DEFAULT_DISTRIBUTION_DIR = "quarkit_distribution";

        public void Build(string manifestPath)
        {
            var manifest = ManifestEngine.Load(manifestPath);
            var manifestDir = Path.GetDirectoryName(manifestPath);
            if (manifest == null || manifestDir == null) {
                Console.WriteLine($"Could not find the Quarkit installer manifest.");
                Console.WriteLine($"--help for more information.\n");
                Console.WriteLine($"Searched in: {manifestDir}");
                Console.WriteLine($"Exact file: {manifest}");
                return;
            }

            string quarkitRoot = AppDomain.CurrentDomain.BaseDirectory; // Where .exe is located (Quakit.CLI.exe is always in root)
            TargetKey hostTarget = PayloadDiscoveryEngine.DiscoverHostTarget();

            ShorthandEngine shorthandEngine = new();
            shorthandEngine.SetToken("<QK>", quarkitRoot);
            shorthandEngine.SetToken("<QkHostTriple>", hostTarget.GetTriple());
            shorthandEngine.SetToken("<QkHostSystem>", hostTarget.System.AsString());
            shorthandEngine.SetToken("<QkHostArchitecture>", hostTarget.Arch.AsString());
            shorthandEngine.SetToken("<QkHostBitness>", hostTarget.Bit.AsString());
            // Potentially: Versions... later

            PhysicalFileSystem physicalFileSystem = new();
            ProcessRunner systemProcessRunner = new();
            InstallationManifestEngine manifestEngine = new();
            PayloadDiscoveryEngine discoveryEngine = new(physicalFileSystem);
            ModulesEngine modulesEngine = new(quarkitRoot, physicalFileSystem, systemProcessRunner);
            BuildEngine buildEngine = new(physicalFileSystem, systemProcessRunner);

            var payloads = discoveryEngine.DiscoverPayloads(manifest.AutoDiscovery?.TargetRootDirectory ?? "", manifest.AutoDiscovery?.TargetPayloadSuffix ?? "");
            foreach (var payload in payloads) {
                if (!IsManifestSupportiveOfTarget(manifest, payload.Target))
                {
                    Console.WriteLine($"Found a target: {payload.FolderName}(interpreted as: {payload.Target.GetTriple()}), but the target is not supported in the manifest. " +
                                        $"If this is intented you can ignore this message.");
                }

                var target = payload.Target;
                InstallOptions resolvedOptions = manifestEngine.ResolveForTarget(manifest, target);
                List<LoadedModule> modules = [];
                if (resolvedOptions.Modules != null) {
                    foreach (var module in resolvedOptions.Modules)
                        modules.Add(modulesEngine.ResolveAndLoadModule(module, manifestDir));
                }

                // TODO: Add a feature into the shorthand engine so it can be cloned/duplicated and there can be
                // custom shorthand values for each target like <QkTargetTriple> ...

                // Run commands that are marked as before build ones.
                foreach (var module in modules) modulesEngine.RunPreBuildCommands(module, shorthandEngine);

                buildEngine.Build(new()
                {
                    ActiveModules = modules,
                    OutputPath = Path.Combine(manifestDir, manifest.OutputPath ?? DEFAULT_DISTRIBUTION_DIR, $"{target.GetTriple()}", $"{manifest.Default.AppName}_qkinstaller.exe"),
                    QuarkitRoot = quarkitRoot,
                    ResolvedOptions = resolvedOptions,
                    Target = target,
                    CompilerName = manifest.CreatorOptions == null ? "clang" : manifest.CreatorOptions.CompilerName,
                    CompilerType = CompilerType.Clang
                });
            }
        }

        private static bool IsManifestSupportiveOfTarget(InstallManifestEditor manifest, TargetKey targetKey)
        {
            if (manifest.SupportedSystems != null && !manifest.SupportedSystems.Contains(targetKey.System)) return false;
            if (manifest.SupportedArchitectures != null && !manifest.SupportedArchitectures.Contains(targetKey.Arch)) return false;
            if (manifest.SupportedBitnesses != null && !manifest.SupportedBitnesses.Contains(targetKey.Bit)) return false;
            return true; // By default if Supported... is null or contains the target value it's supportive.
        }
    }
}
