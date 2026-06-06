namespace Quarkit.Models.Manifest.Installer
{
    public class InstallerOptionsOverrides : IBlueprintOverride<InstallerOptions>
    {   
        /// <summary>
        /// Overrides to be applied for the specified target.
        /// </summary>
        public required InstallerOptions Value { get; set; }

        /// <summary>
        /// Specifies which targets should these overrides handle.
        /// </summary>
        public TargetKey? TargetKey { get; set; }
        public List<string>? Expressions { get; set; }
    }
}
