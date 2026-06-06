using Quarkit.Core.Manifest;
using Quarkit.Models.Manifest;
using Quarkit.Models.Manifest.Installer;

namespace Quarkit.Tests;

public class InstallationManifestOverridesTests
{
    [Test]
    public async Task CascadingOverrides_ShouldLayerPropertiesProgressively()
    {
        var manifest = new InstallerManifest
        {
            Default = new InstallerBlueprint
            {
                AppName = "CoreApp",
                AdminRequired = false,
                DesktopShortcut = true,
                TargetPath = "/default/path"
            },
            Overrides =
            [
                // Matches ANY Windows target
                new()
                {
                    TargetKey = new TargetKey(OSSystem.Windows),
                    Value = new(){
                        AdminRequired = true
                    }
                },
                // Matches ANY 64-bit target
                new()
                {
                    TargetKey = new TargetKey(bit: Bitness.x64),
                    Value = new(){
                        TargetPath = "/programfiles64/CoreApp"
                    }
                },
                // Very specific target
                new()
                {
                    TargetKey = new TargetKey(OSSystem.Windows, Architecture.x86, Bitness.x64),
                    Value = new(){
                        DesktopShortcut = false
                    }
                }
            ]
        };

        OverridesResolver resolver = new OverridesResolver();
        TargetKey concreteTarget = new(OSSystem.Windows, Architecture.x86, Bitness.x64);
        InstallerBlueprint? resolved = resolver.ResolveForTarget(manifest.Default, manifest.Overrides.ToArray(), concreteTarget, null);

        await Assert.That(resolved).IsNotNull();
        await Assert.That(resolved.AppName).IsEqualTo("CoreApp"); // From Default
        await Assert.That(resolved.AdminRequired).IsTrue(); // From 1st Override 
        await Assert.That(resolved.TargetPath).IsEqualTo("/programfiles64/CoreApp"); // From 2nd Override
        await Assert.That(resolved.DesktopShortcut).IsFalse(); // From 3rd Override
    }

    [Test]
    public async Task ResolveForTarget_WhenNoOverridesMatch_ShouldReturnDefaults()
    {
        var manifest = new InstallerManifest
        {
            Default = new InstallerBlueprint { AppName = "BaseOnly", AdminRequired = false },
            Overrides =
            [
                new() { TargetKey = new TargetKey(OSSystem.Linux), Value = new(){ AdminRequired = true } }
            ]
        };

        OverridesResolver resolver = new();
        TargetKey concreteWindowsTarget = new(OSSystem.Windows, Architecture.x86, Bitness.x64);
        InstallerBlueprint defaultOptions = InstallerBlueprint.GetGlobalDefaults();
        defaultOptions.MergeFrom(manifest.Default);
        InstallerBlueprint? resolved = resolver.ResolveForTarget(defaultOptions, manifest.Overrides.ToArray(), concreteWindowsTarget, null);

        await Assert.That(resolved).IsNotNull();
        await Assert.That(resolved.AdminRequired).IsFalse();
        await Assert.That(resolved.AppName).IsEqualTo("BaseOnly");
    }
}