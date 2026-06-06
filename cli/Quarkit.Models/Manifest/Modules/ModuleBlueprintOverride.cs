using Quarkit.Models.Core.Target;

namespace Quarkit.Models.Manifest.Modules
{
    public class ModuleBlueprintOverride : IBlueprintOverride<ModuleBlueprint>
    {
        public required ModuleBlueprint Value { get; set; }
        public TargetKey? TargetKey { get; set; }
        public List<string>? Expressions { get; set; }
    }
}
