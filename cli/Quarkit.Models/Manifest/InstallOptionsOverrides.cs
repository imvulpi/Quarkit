namespace Quarkit.Models.Manifest
{

    public class InstallOptionsOverrides
    {   
        /// <summary>
        /// Specifies which targets should these overrides handle.
        /// </summary>
        public required TargetKey TargetKey { get; set; }

        /// <summary>
        /// Overrides to be applied for the specified target.
        /// </summary>
        public InstallOptions? Options { get; set; }        
    }
}
