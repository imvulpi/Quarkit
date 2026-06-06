namespace Quarkit.Models.Manifest.Installer
{
    public static class InstallerOptionsHelpers
    {

        public static InstallerOptions MergeWith(this InstallerOptions baseOptions, InstallerOptions? overrideOptions)
        {
            if (overrideOptions == null) return baseOptions;

            return new InstallerOptions
            {
                AppName = overrideOptions.AppName ?? baseOptions.AppName,
                DesktopShortcut = overrideOptions.DesktopShortcut ?? baseOptions.DesktopShortcut,
                StartShortcut = overrideOptions.StartShortcut ?? baseOptions.StartShortcut,
                AdminRequired = overrideOptions.AdminRequired ?? baseOptions.AdminRequired,
                ExecutableToLaunch = overrideOptions.ExecutableToLaunch ?? baseOptions.ExecutableToLaunch,
                TargetPath = overrideOptions.TargetPath ?? baseOptions.TargetPath,
            };
        }
    }
}