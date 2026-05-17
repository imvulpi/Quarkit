namespace Quarkit.Models.Manifest
{
    /// <summary>
    /// Options on how should the installation be performed.
    /// </summary>
    public class InstallOptions
    {
        public string? AppId { get; set; }

        /// <summary>
        /// Whether a desktop shorcut should be created, if the system supports it.
        /// </summary>
        public bool? DesktopShortcut { get; set; } = true;

        /// <summary>
        /// Whether a shortcut should be created in the start menu, if the system supports it.
        /// </summary>
        public bool? StartShortcut { get; set; } = true;

        /// <summary>
        /// Whether admin permissions are required for the installer.
        /// </summary>
        public bool? AdminRequired { get; set; } = false;

        /// <summary>
        /// Path to the executable to be run after extraction.
        /// </summary>
        public string? ExecutableToLaunch { get; set; }

        /// <summary>
        /// Path to the directory or file which should be packed and extracted when installed.
        /// </summary>
        public string? TargetPath { get; set; }
    }
}
