using Quarkit.Models.Core;
using Quarkit.Models.Manifest;

namespace Quarkit.Core.Manifest;

public class OverridesResolver
{
    /// <summary>
    /// Resolves the absolute, flat configuration for a single concrete target 
    /// by layering matching overrides on top of the defaults.
    /// </summary>
    public T ResolveForTarget<T>(T defaultT, IBlueprintOverride<T>[] overrides, TargetKey targetKey, List<QkOptionDefinition>? availableOptions)
        where T : IMergeable<T>
    {
        foreach (var blueprintOverride in overrides)
        {
            if ((blueprintOverride.TargetKey.HasValue && IsMatch(targetKey, blueprintOverride.TargetKey.Value))
                || (availableOptions != null && IsMatch(blueprintOverride.Expressions, availableOptions)))
            {
                defaultT.MergeFrom(blueprintOverride.Value);
            }
        }

        return defaultT;
    }

    private bool IsMatch(List<string>? expressions, List<QkOptionDefinition> availableOptions)
    {
        if (expressions == null || expressions.Count == 0) return true;

        QkOptionResolver qkOptionResolver = new(availableOptions);
        return qkOptionResolver.ExpressionsMatch(expressions);
    }

    private bool IsMatch(TargetKey concreteTarget, TargetKey overrideFilter)
    {
        if (overrideFilter.System != OSSystem.None && overrideFilter.System != concreteTarget.System) return false;
        if (overrideFilter.Arch != Architecture.None && overrideFilter.Arch != concreteTarget.Arch) return false;
        if (overrideFilter.Bit != Bitness.None && overrideFilter.Bit != concreteTarget.Bit) return false;
        return true;
    }
}
