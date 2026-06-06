namespace Quarkit.Models.Manifest
{
    /// <summary>
    /// Options on how should the installation be performed.
    /// </summary>
    public class InstallOptions : IMergeable<InstallOptions>
    {
        /// <summary>
        /// Name of the application.
        /// </summary>
        public string? AppName { get; set; }

        /// <summary>
        /// Whether a desktop shorcut should be created, if the system supports it.
        /// </summary>
        public bool? DesktopShortcut { get; set; }

        /// <summary>
        /// Whether a shortcut should be created in the start menu, if the system supports it.
        /// </summary>
        public bool? StartShortcut { get; set; }

        /// <summary>
        /// Whether admin permissions are required for the installer.
        /// </summary>
        public bool? AdminRequired { get; set; }

        /// <summary>
        /// Path to the executable to be run after extraction.
        /// </summary>
        public string? ExecutableToLaunch { get; set; }

        /// <summary>
        /// Modules to be included in these installation configuration.
        /// </summary>
        /// <remarks> 
        /// You can use &lt;QK&gt;/ shorthand to provide an explicit path relative to the Quarkit root,
        /// otherwise ./ is relative to the manifest file.<br></br> 
        /// 
        /// If no directory is found from the provided string then quarkit will attempt to find the module
        /// based on its id or directory from common places.
        /// </remarks>
        public List<string>? Modules { get; set; }

        /// <summary>
        /// Path to the directory or file which should be packed and extracted when installed.
        /// </summary>
        public string? TargetPath { get; set; }

        /// <summary>
        /// Provides the absolute baseline defaults for Quarkit when nothing else is specified.
        /// </summary>
        public static InstallOptions GetGlobalDefaults() => new()
        {
            AppName = "MyQuarkitApp",
            DesktopShortcut = true,
            StartShortcut = true,
            AdminRequired = false,
            ExecutableToLaunch = null,
            TargetPath = null,
            Modules = null
        };

        /// <summary>
        /// Layers properties from a source options object onto this instance, 
        /// ignoring any null values in the source.
        /// </summary>
        public void MergeFrom(InstallOptions? source)
        {
            if (source == null) return;

            if (source.AppName != null) AppName = source.AppName;
            if (source.DesktopShortcut != null) DesktopShortcut = source.DesktopShortcut;
            if (source.StartShortcut != null) StartShortcut = source.StartShortcut;
            if (source.AdminRequired != null) AdminRequired = source.AdminRequired;
            if (source.ExecutableToLaunch != null) ExecutableToLaunch = source.ExecutableToLaunch;
            if (source.TargetPath != null) TargetPath = source.TargetPath;
            if (source.Modules != null) Modules = source.Modules;
        }

        /// <summary>
        /// Helper to create a deep copy of an option set.
        /// </summary>
        public InstallOptions Clone()
        {
            var clone = new InstallOptions();
            clone.MergeFrom(this);
            return clone;
        }
    }
}
