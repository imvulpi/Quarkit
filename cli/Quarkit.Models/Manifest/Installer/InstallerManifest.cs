namespace Quarkit.Models.Manifest.Installer
{
    public class AutoDiscovery
    {
        /// <summary>
        /// (Optional) The folder containing framework-compiled target triplets for auto-discovery.
        /// </summary>
        public required string TargetRootDirectory { get; set; }

        /// <summary>
        /// (Optional) A relative directory appended to discovered target folders to reach deployment binaries.
        /// </summary>
        public string? TargetPayloadSuffix { get; set; } // e.g., "/publish" or "/bin"
    }

    public class InstallerManifest
    {
        public InstallerCreatorOptions? CreatorOptions { get; set; }

        /// <summary>
        /// Options for auto discovery of targets.
        /// </summary>
        public AutoDiscovery? AutoDiscovery { get; set; }

        /// <summary>
        /// (Optional) Path to where the installers should be outputted to.
        /// </summary>
        public string? OutputPath { get; set; }

        /// <summary>
        /// (Optional) Specifies which systems are supported and should generate installers for. <br />
        /// On default it assumes support for all systems.
        /// </summary>
        public List<OSSystem>? SupportedSystems { get; set; }

        /// <summary>
        /// (Optional) Specifies which cpu architectures are supported.<br />
        /// On default it assumes support for all architectures.
        /// </summary>
        public List<Architecture>? SupportedArchitectures { get; set; }

        /// <summary>
        /// (Optional) Specifies the supported bitness.<br />
        /// On default it assumes support for all architectures.
        /// </summary>
        public List<Bitness>? SupportedBitnesses { get; set; }

        /// <summary>
        /// The base configuration applied to every installer variant.
        /// </summary>
        public required InstallerOptions Default { get; set; }

        /// <summary>
        /// A collection of selective updates applied to specific OS or architectural matrices.
        /// </summary>
        public List<InstallerOptionsOverrides> Overrides { get; set; } = [];  
    }
}
