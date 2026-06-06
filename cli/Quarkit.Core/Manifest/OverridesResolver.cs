using Quarkit.Models.Core.Target;
using Quarkit.Models.Manifest;

namespace Quarkit.Core.Manifest;

public class OverridesResolver
{
    /// <summary>
    /// Resolves the absolute, flat configuration for a single concrete target 
    /// by layering matching overrides on top of the defaults.
    /// </summary>
    public T ResolveForTarget<T>(T defaultT, IBlueprintOverride<T>[] overrides, TargetKey targetKey, QkOptionResolver? qkOptionResolver = null)
        where T : IMergeable<T>
    {
        foreach (var blueprintOverride in overrides)
        {
            bool noTargetOrTargetMatch = !blueprintOverride.TargetKey.HasValue || (blueprintOverride.TargetKey.HasValue && IsMatch(targetKey, blueprintOverride.TargetKey.Value));
            bool noOptionsOrOptionsMatch = qkOptionResolver == null || IsMatch(blueprintOverride.Expressions, qkOptionResolver);
            if (noTargetOrTargetMatch && noOptionsOrOptionsMatch)
            {
                defaultT.MergeFrom(blueprintOverride.Value);
            }
        }

        return defaultT;
    }

    private bool IsMatch(List<string>? expressions, QkOptionResolver resolver)
    {
        if (expressions == null || expressions.Count == 0) return true;
        return resolver.ExpressionsMatch(expressions);
    }

    private bool IsMatch(TargetKey concreteTarget, TargetKey overrideFilter)
    {
        if (overrideFilter.System != OSSystem.None && overrideFilter.System != concreteTarget.System) return false;
        if (overrideFilter.Arch != Architecture.None && overrideFilter.Arch != concreteTarget.Arch) return false;
        if (overrideFilter.Bit != Bitness.None && overrideFilter.Bit != concreteTarget.Bit) return false;
        return true;
    }
}
