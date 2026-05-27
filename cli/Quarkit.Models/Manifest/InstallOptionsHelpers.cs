namespace Quarkit.Models.Manifest
{
    public static class InstallOptionsHelpers
    {

        public static InstallOptions MergeWith(this InstallOptions baseOptions, InstallOptions? overrideOptions)
        {
            if (overrideOptions == null) return baseOptions;

            return new InstallOptions
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