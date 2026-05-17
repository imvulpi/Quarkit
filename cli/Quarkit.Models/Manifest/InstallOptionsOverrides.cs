namespace Quarkit.Models.Manifest
{
    public class InstallOptionsOverrides
    {
        /// <summary>
        /// The target system.
        /// </summary>
        public OperatingSystem? System { get; set; }
        
        /// <summary>
        /// The target architecture.
        /// </summary>
        public Architecture? Architecture { get; set; }
        
        /// <summary>
        /// The target bitness.
        /// </summary>
        public Bitness? Bitness { get; set; }
        
        /// <summary>
        /// Overrides to be applied for the specified target.
        /// </summary>
        public InstallOptions? Options { get; set; }
        
        /// <summary>
        /// Path to the directory or file which should be packed and extracted when installed.
        /// </summary>
        public string? TargetPath { get; set; }
    }
}
