using Quarkit.Models.Core.Target;

namespace Quarkit.Models.Manifest.Installer
{
    public class InstallerBlueprintOverrides : IBlueprintOverride<InstallerBlueprint>
    {
        /// <summary>
        /// Overrides to be applied for the specified target.
        /// </summary>
        public required InstallerBlueprint Value { get; set; }

        /// <summary>
        /// Specifies which targets should these overrides handle.
        /// </summary>
        public TargetKey? TargetKey { get; set; }
        public List<string>? Expressions { get; set; }
    }
}
