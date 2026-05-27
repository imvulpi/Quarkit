using Quarkit.Core.Installation;
using Quarkit.Models.Manifest;

namespace Quarkit.Tests;

public class InstallationManifestEngineTests
{
    private readonly InstallationManifestEngine _engine = new();

    [Test]
    public async Task CascadingOverrides_ShouldLayerPropertiesProgressively()
    {
        var manifest = new InstallManifestEditor
        {
            Default = new InstallOptions
            {
                AppName = "CoreApp",
                AdminRequired = false,
                DesktopShortcut = true,
                TargetPath = "/default/path"
            },
            Overrides = new List<InstallOptionsOverrides>
            {
                // Matches ANY Windows target
                new()
                {
                    TargetKey = new TargetKey(OSSystem.Windows),
                    Options = new(){
                        AdminRequired = true
                    }
                },
                // Matches ANY 64-bit target
                new()
                {
                    TargetKey = new TargetKey(bit: Bitness.x64),
                    Options = new(){
                        TargetPath = "/programfiles64/CoreApp"
                    }
                },
                // Very specific target
                new()
                {
                    TargetKey = new TargetKey(OSSystem.Windows, Architecture.x86, Bitness.x64),
                    Options = new(){
                        DesktopShortcut = false
                    }
                }
            }
        };

        var concreteTarget = new TargetKey(OSSystem.Windows, Architecture.x86, Bitness.x64);
        var resolved = _engine.ResolveForTarget(manifest, concreteTarget);

        await Assert.That(resolved.AppName).IsEqualTo("CoreApp"); // From Default
        await Assert.That(resolved.AdminRequired).IsTrue(); // From 1st Override 
        await Assert.That(resolved.TargetPath).IsEqualTo("/programfiles64/CoreApp"); // From 2nd Override
        await Assert.That(resolved.DesktopShortcut).IsFalse(); // From 3rd Override
    }

    [Test]
    public async Task ResolveForTarget_WhenNoOverridesMatch_ShouldReturnDefaults()
    {
        var manifest = new InstallManifestEditor
        {
            Default = new InstallOptions { AppName = "BaseOnly", AdminRequired = false },
            Overrides = new List<InstallOptionsOverrides>
            {
                new() { TargetKey = new TargetKey(OSSystem.Linux), Options = new(){ AdminRequired = true } }
            }
        };

        var concreteWindowsTarget = new TargetKey(OSSystem.Windows, Architecture.x86, Bitness.x64);
        var resolved = _engine.ResolveForTarget(manifest, concreteWindowsTarget);

        await Assert.That(resolved.AdminRequired).IsFalse();
        await Assert.That(resolved.AppName).IsEqualTo("BaseOnly");
    }
}