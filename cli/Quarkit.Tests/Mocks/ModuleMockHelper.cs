using Quarkit.Models.Manifest.Modules;

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
    }
}
