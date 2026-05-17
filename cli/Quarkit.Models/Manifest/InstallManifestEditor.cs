namespace Quarkit.Models.Manifest
{
    public class InstallManifestEditor
    {
        /// <summary>
        /// (Optional) The folder containing framework-compiled target triplets for auto-discovery.
        /// </summary>
        public string? TargetRootDirectory { get; set; }

        /// <summary>
        /// (Optional) A relative directory appended to discovered target folders to reach deployment binaries.
        /// </summary>
        public string? TargetPayloadSuffix { get; set; } // e.g., "/publish" or "/bin"

        /// <summary>
        /// (Optional) Specifies which systems are supported and should generate installers for.
        /// </summary>
        public List<OperatingSystem>? SupportedSystems { get; set; }

        /// <summary>
        /// (Optional) Specifies which cpu architectures are supported.
        /// </summary>
        public List<Architecture>? SupportedArchitectures { get; set; }

        /// <summary>
        /// (Optional) Specifies the supported bitness.
        /// </summary>
        public List<Bitness>? SupportedBitnesses { get; set; }

        /// <summary>
        /// The base configuration applied to every installer variant.
        /// </summary>
        public required InstallOptions Default { get; set; }

        /// <summary>
        /// A collection of selective updates applied to specific OS or architectural matrices.
        /// </summary>
        public List<InstallOptionsOverrides> Overrides { get; set; } = [];
    }
}
