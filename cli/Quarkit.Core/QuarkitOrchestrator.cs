using Quarkit.Core.Build;
using Quarkit.Core.Discovery;
using Quarkit.Core.Manifest;
using Quarkit.Core.Modules;
using Quarkit.Core.Processes;
using Quarkit.Core.Shorthand;
using Quarkit.Core.Storage;
using Quarkit.Models.Core.Target;
using Quarkit.Models.Manifest.Installer;
using Quarkit.Models.Manifest.Modules;
using System.Text.Json;

namespace Quarkit.Core
{
    public class QuarkitOrchestrator
    {
        public const string DEFAULT_DISTRIBUTION_DIR = "quarkit_distribution";

        public void Build(string manifestPath)
        {
            if (!File.Exists(manifestPath))
            {
                Console.WriteLine($"Could not find the manifest in {manifestPath}");
                return;
            }

            var manifest = JsonSerializer.Deserialize<InstallerManifest>(File.ReadAllText(manifestPath));
            var manifestDir = Path.GetDirectoryName(manifestPath);
            if (manifest == null || manifestDir == null)
            {
                Console.WriteLine($"Could not find the Quarkit installer manifest.");
                Console.WriteLine($"--help for more information.\n");
                Console.WriteLine($"Searched in: {manifestDir}");
                Console.WriteLine($"Exact file: {manifest}");
                return;
            }
            Console.WriteLine("Found a manifest.");

            string quarkitRoot = AppDomain.CurrentDomain.BaseDirectory; // Where .exe is located (Quakit.CLI.exe is always in root)
            TargetKey hostTarget = PayloadDiscoveryEngine.DiscoverHostTarget();

            ShorthandEngine globalShorthandEngine = new();
            globalShorthandEngine.SetToken("<QK>", quarkitRoot);
            globalShorthandEngine.SetToken("<QkHostTriple>", hostTarget.GetTriple());
            globalShorthandEngine.SetToken("<QkHostSystem>", hostTarget.System.AsString());
            globalShorthandEngine.SetToken("<QkHostArchitecture>", hostTarget.Arch.AsString());
            globalShorthandEngine.SetToken("<QkHostBitness>", hostTarget.Bit.AsString());
            // Potentially: Versions... later

            PhysicalFileSystem physicalFileSystem = new();
            ProcessRunner systemProcessRunner = new();
            PayloadDiscoveryEngine discoveryEngine = new(physicalFileSystem);
            ModulesEngine modulesEngine = new(quarkitRoot, physicalFileSystem, systemProcessRunner);
            BuildEngine buildEngine = new(physicalFileSystem, systemProcessRunner);
            OverridesResolver resolver = new();

            var payloads = discoveryEngine.DiscoverPayloads(manifest.AutoDiscovery?.TargetRootDirectory ?? "", manifest.AutoDiscovery?.TargetPayloadSuffix ?? "");
            if(payloads.Count == 0)
            {
                Console.WriteLine("... No targets found!");
            }

            foreach (var payload in payloads)
            {
                Console.WriteLine($"\nBuilding the installer for {payload.Target.GetTriple()}.");
                if (!IsManifestSupportiveOfTarget(manifest, payload.Target))
                {
                    Console.WriteLine($"Found a target: {payload.FolderName}(interpreted as: {payload.Target.GetTriple()}), but the target is not supported in the manifest. " + "If this is intented you can ignore this message.");
                }

                ShorthandEngine buildShorthandEngine = new(globalShorthandEngine);
                TargetKey target = payload.Target;
                string coreModuleDir = Path.Combine(quarkitRoot, Paths.GetInstallerDir(target.System));

                InstallerBlueprint? resolvedOptions = resolver.ResolveForTarget(manifest.Default, manifest.Overrides.ToArray(), target, new(new Dictionary<string, Models.Core.QkOptionDefinition>()));
                string scratchDir = Path.Join(quarkitRoot, "build", target.GetTriple());

                string payloadName = GetPayloadName(payload);
                Directory.CreateDirectory(scratchDir);
                buildShorthandEngine.SetToken("<PayloadPath>", payload.AbsolutePayloadPath);
                buildShorthandEngine.SetToken("<PayloadName>", payloadName);
                buildShorthandEngine.SetToken("<ScratchPath>", scratchDir);
                Console.WriteLine($"Target: {payload.AbsolutePayloadPath}");

                string[] moduleIds;
                if(resolvedOptions.Modules != null)
                {
                    moduleIds = new string[resolvedOptions.Modules.Count + 1];
                    moduleIds[0] = $"<QK>/{Paths.GetInstallerDir(target.System)}";
                    Array.Copy(resolvedOptions.Modules.ToArray(), 0, moduleIds, 1, resolvedOptions.Modules.Count);
                }
                else
                {
                    moduleIds = new string[1];
                    moduleIds[0] = $"<QK>/{Paths.GetInstallerDir(target.System)}";
                }

                ResolvedModule[] resolvedModules = modulesEngine.ResolveModules(moduleIds, manifestPath, target, resolver);

                string modulesVerbose = "";
                // Run commands that are marked as before build ones.
                foreach (var resolved in resolvedModules)
                {
                    modulesVerbose += resolved.Module.Manifest.Id + ", ";
                    modulesEngine.RunPreBuildCommands(resolved.Module, resolved.Blueprint, buildShorthandEngine);
                }

                Console.WriteLine($"Modules: {modulesVerbose}");

                buildEngine.Build(new()
                {
                    ResolvedModules = resolvedModules,
                    OutputPath = Path.Combine(manifestDir, manifest.OutputPath ?? DEFAULT_DISTRIBUTION_DIR, $"{target.GetTriple()}", $"{manifest.Default.AppName}_qkinstaller.exe"),
                    QuarkitRoot = quarkitRoot,
                    ResolvedOptions = resolvedOptions,
                    PayloadPath = buildShorthandEngine.Expand("<PayloadPath>"),
                    PayloadName = buildShorthandEngine.Expand("<PayloadName>"),
                    Target = target,
                    CompilerName = manifest.CreatorOptions == null ? "clang" : manifest.CreatorOptions.CompilerName,
                    CompilerType = CompilerType.Clang
                });
            }
        }

        private static string GetPayloadName(DiscoveredPayload payload, string defaultName = "payload")
        {
            FileAttributes attr = File.GetAttributes(payload.AbsolutePayloadPath);
            if (attr.HasFlag(FileAttributes.Directory))
            {
                return Path.GetDirectoryName(payload.AbsolutePayloadPath) ?? defaultName;
            }
            else
            {
                return Path.GetFileName(payload.AbsolutePayloadPath) ?? defaultName;
            }
        }

        private static bool IsManifestSupportiveOfTarget(InstallerManifest manifest, TargetKey targetKey)
        {
            if (manifest.SupportedSystems != null && !manifest.SupportedSystems.Contains(targetKey.System)) return false;
            if (manifest.SupportedArchitectures != null && !manifest.SupportedArchitectures.Contains(targetKey.Arch)) return false;
            if (manifest.SupportedBitnesses != null && !manifest.SupportedBitnesses.Contains(targetKey.Bit)) return false;
            return true; // By default if Supported... is null or contains the target value it's supportive.
        }
    }
}
