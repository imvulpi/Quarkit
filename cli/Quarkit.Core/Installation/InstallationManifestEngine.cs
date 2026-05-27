using Quarkit.Models.Manifest;

namespace Quarkit.Core.Installation;

public class InstallationManifestEngine
{
    /// <summary>
    /// Resolves the absolute, flat configuration for a single concrete target 
    /// by layering matching overrides on top of the defaults.
    /// </summary>
    public InstallOptions ResolveForTarget(InstallManifestEditor manifest, TargetKey targetKey)
    {
        var resolvedOptions = InstallOptions.GetGlobalDefaults();
        if (manifest.Default != null)
        {
            resolvedOptions.MergeFrom(manifest.Default);
        }

        foreach (var currentOverride in manifest.Overrides)
        {
            if (currentOverride.Options != null && IsMatch(targetKey, currentOverride.TargetKey))
            {
                resolvedOptions.MergeFrom(currentOverride.Options);
            }
        }

        return resolvedOptions;
    }

    private bool IsMatch(TargetKey concreteTarget, TargetKey overrideFilter)
    {
        if (overrideFilter.System != OSSystem.None && overrideFilter.System != concreteTarget.System) return false;
        if (overrideFilter.Arch != Architecture.None && overrideFilter.Arch != concreteTarget.Arch) return false;
        if (overrideFilter.Bit != Bitness.None && overrideFilter.Bit != concreteTarget.Bit) return false;
        return true;
    }
}