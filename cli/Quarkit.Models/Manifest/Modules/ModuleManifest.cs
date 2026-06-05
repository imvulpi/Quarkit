using Quarkit.Models.Core;

namespace Quarkit.Models.Manifest.Modules
{
    public class ModuleManifest
    {
        public required string Id { get; set; } 
        public required string Version { get; set; }
        public List<QkOptionDefinition>? Options { get; set; }
        public ModuleBlueprint Default { get; set; } = new();
        public List<ModuleBlueprintOverride>? Overrides { get; set; }
    }
}
