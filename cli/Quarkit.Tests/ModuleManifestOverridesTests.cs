using Quarkit.Core.Manifest;
using Quarkit.Models.Core;
using Quarkit.Models.Core.Target;
using Quarkit.Models.Manifest;
using Quarkit.Models.Manifest.Modules;

namespace Quarkit.Tests
{
    public class ModuleManifestOverridesTests
    {
        #region Scenario 1: TargetKey-Only Overrides (No Options)

        [Test]
        public async Task ResolveForTarget_WithStaticTargetKeys_ShouldLayerPropertiesProgressively()
        {
            QkOptionResolver resolver = new(new List<QkOptionDefinition>());
            var defaultBlueprint = new ModuleBlueprint
            {
                HasInitHook = false,
                Dependencies = new List<string> { "CoreLib" },
                CompilerFlags = new List<string> { "-O2" }
            };

            var overrides = new IBlueprintOverride<ModuleBlueprint>[]
            {
                new ModuleBlueprintOverride
                {
                    TargetKey = new TargetKey(OSSystem.Windows),
                    Value = new ModuleBlueprint
                    {
                        HasInitHook = true,
                        Dependencies = new ModuleDelta { Add = new List<string> { "Win32Registry" } },
                        CompilerFlags = new List<string> { "-O3" } // Full overwrite of flags
                    }
                },
                new ModuleBlueprintOverride
                {
                    TargetKey = new TargetKey(bit: Bitness.x64),
                    Value = new ModuleBlueprint
                    {
                        CompilerFlags = new ModuleDelta { Add = new List<string> { "-msse4.2" } }
                    }
                }
            };

            var activeTarget = new TargetKey(OSSystem.Windows, Architecture.x86, Bitness.x64);
            var availableOptions = new List<QkOptionDefinition>();
            OverridesResolver overridesResolver = new();

            var result = overridesResolver.ResolveForTarget(defaultBlueprint, overrides, activeTarget, resolver);

            await Assert.That(result.HasInitHook).IsTrue();

            await Assert.That(result.Dependencies?.Values).Contains("CoreLib");
            await Assert.That(result.Dependencies?.Values).Contains("Win32Registry");

            await Assert.That(result.CompilerFlags?.Values).DoesNotContain("-O2");
            await Assert.That(result.CompilerFlags?.Values).Contains("-O3");
            await Assert.That(result.CompilerFlags?.Values).Contains("-msse4.2");
        }

        #endregion

        #region Scenario 2: Dynamic Dynamic Option-Based Overrides

        [Test]
        public async Task ResolveForTarget_WithMatchingOptions_ShouldApplyDeltas()
        {
            QkOptionResolver resolver = new(new List<QkOptionDefinition> {
                new() { Name = "MaxPlayers", Type = QkType.Number }
            });
            //resolver.ResolveExpression("MaxPlayers=150");
            resolver.OptionAndValues["MaxPlayers"] = "150";

            var defaultBlueprint = new ModuleBlueprint
            {
                Dependencies = new List<string> { "StandardNet" }
            };

            var overrides = new IBlueprintOverride<ModuleBlueprint>[]
            {
                new ModuleBlueprintOverride
                {
                    Expressions = new List<string> { "MaxPlayers>100" },
                    Value = new ModuleBlueprint
                    {
                        Dependencies = new ModuleDelta
                        {
                            Add = new List<string> { "HighCapacitySockets" },
                            Remove = new List<string> { "StandardNet" }
                        }
                    }
                }
            };

            var target = new TargetKey(OSSystem.Linux);
            OverridesResolver overridesResolver = new();

            var result = overridesResolver.ResolveForTarget(defaultBlueprint, overrides, target, resolver);

            await Assert.That(result.Dependencies?.Values).DoesNotContain("StandardNet");
            await Assert.That(result.Dependencies?.Values).Contains("HighCapacitySockets");
        }

        [Test]
        public async Task ResolveForTarget_WithNonMatchingOptions_ShouldIgnoreOverrideBlock()
        {
            QkOptionResolver resolver = new(new List<QkOptionDefinition>
            {
                new() { Name = "UseUltraSettings", Type = QkType.Boolean }
            });
            resolver.OptionAndValues["UseUltraSettings"] = "false";

            var defaultBlueprint = new ModuleBlueprint
            {
                CSources = new List<string> { "main.c" }
            };

            var overrides = new IBlueprintOverride<ModuleBlueprint>[]
            {
                new ModuleBlueprintOverride
                {
                    Expressions = new List<string> { "UseUltraSettings==true" },
                    Value = new ModuleBlueprint
                    {
                        CSources = new ModuleDelta { Add = new List<string> { "raytrace_pipeline.c" } }
                    }
                }
            };
            OverridesResolver overridesResolver = new();

            var result = overridesResolver.ResolveForTarget(defaultBlueprint, overrides, new TargetKey(OSSystem.Linux), resolver);

            await Assert.That(result.CSources?.Values).Contains("main.c");
            await Assert.That(result.CSources?.Values).DoesNotContain("raytrace_pipeline.c");
        }

        #endregion

        #region Scenario 3: Mixed Targets and Options Cascading

        [Test]
        public async Task ResolveForTarget_WithMixedTargetKeysAndOptions_ShouldChainCorrectly()
        {
            QkOptionResolver resolver = new(new List<QkOptionDefinition>
            {
                new() { Name = "EnableExperimental", Type = QkType.Boolean }
            });
            resolver.OptionAndValues["EnableExperimental"] = "true";

            var defaultBlueprint = new ModuleBlueprint
            {
                CompilerFlags = new List<string> { "-O2" }
            };

            var overrides = new IBlueprintOverride<ModuleBlueprint>[]
            {
                new ModuleBlueprintOverride
                {
                    TargetKey = new TargetKey(OSSystem.Windows),
                    Value = new ModuleBlueprint
                    {
                        CompilerFlags = new ModuleDelta { Add = new List<string> { "-D_WINDOWS" } }
                    }
                },
                new ModuleBlueprintOverride
                {
                    Expressions = new List<string> { "EnableExperimental==true" },
                    Value = new ModuleBlueprint
                    {
                        CompilerFlags = new ModuleDelta { Add = new List<string> { "-experimental-flag" } }
                    }
                }
            };
            OverridesResolver overridesResolver = new();

            var windowsTarget = new TargetKey(OSSystem.Windows, Architecture.x86, Bitness.x64);

            var result = overridesResolver.ResolveForTarget(defaultBlueprint, overrides, windowsTarget, resolver);

            await Assert.That(result.CompilerFlags?.Values).Contains("-O2");
            await Assert.That(result.CompilerFlags?.Values).Contains("-D_WINDOWS");
            await Assert.That(result.CompilerFlags?.Values).Contains("-experimental-flag");
        }

        #endregion
    }
}
