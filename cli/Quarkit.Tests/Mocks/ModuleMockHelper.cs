using Quarkit.Core.Storage;
using Quarkit.Models.Manifest.Modules;
using System.Text.Json;

namespace Quarkit.Tests.Mocks
{
    public static class ModuleMockHelper
    {
        public static List<ResolvedModule> GetResolvedModulesFromDefaults(List<LoadedModule> loadedModules)
        {
            List<ResolvedModule> resolvedModules = [];
            foreach (var module in loadedModules)
            {
                resolvedModules.Add(new ResolvedModule() { Blueprint = module.Manifest.Default, Module = module });
            }
            return resolvedModules;
        }

        public static LoadedModule CreateMockLoadedModule(string id, List<string> dependencies, string moduleDir, string moduleManifestPath, IFileSystem fs)
        {
            var manifest = new ModuleManifest
            {
                Id = id,
                Version = "1.0.0",
                Default = {
                    Dependencies = dependencies
                }
            };

            fs.WriteAllText(moduleManifestPath, JsonSerializer.Serialize(manifest));

            return new LoadedModule()
            {
                Manifest = manifest,
                ManifestPath = moduleManifestPath,
                ModuleDirectory = moduleDir
            };
        }
    }
}
