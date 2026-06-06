namespace Quarkit.Models.Manifest
{

    public class InstallOptionsOverrides : IBlueprintOverride<InstallOptions>
    {   
        /// <summary>
        /// Overrides to be applied for the specified target.
        /// </summary>
        public required InstallOptions Value { get; set; }

        /// <summary>
        /// Specifies which targets should these overrides handle.
        /// </summary>
        public TargetKey? TargetKey { get; set; }
        public List<string>? Expressions { get; set; }
    }
}
