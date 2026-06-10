using Quarkit.Core.Manifest;
using Quarkit.Core.Modules;
using Quarkit.Core.Shorthand;
using Quarkit.Models.Core.Target;
using Quarkit.Models.Manifest.Modules;
using Quarkit.Tests.Mocks;
using System.Text.Json;
using System.Text.Json.Serialization;

namespace Quarkit.Tests;

public class ModulesEngineTests
{
    [Test]
    public async Task ResolveAndLoadModule_WithImplicitCoreFallback_ShouldReadFromQkRoot()
    {
        var mockFs = new MockFileSystem();
        var mockRunner = new MockProcessRunner();
        string qkRoot = "C:/QuarkitCore";
        string manifestDir = "C:/UserProject";

        mockFs.CreateDirectory($"{qkRoot}/modules/brieflz");
        mockFs.WriteAllText($"{qkRoot}/modules/brieflz/module.json",
            JsonSerializer.Serialize(new ModuleManifest()
            {
                Id = "brieflz",
                Version = "1.2.3",
                Default = {
                    CSources = new(["src/brieflz.c"])
                }
            })
        );

        var engine = new ModulesEngine(qkRoot, mockFs, mockRunner);
        var loadedModule = engine.FindAndLoadModule("brieflz", manifestDir);

        await Assert.That(loadedModule.Manifest.Id).IsEqualTo("brieflz");
        await Assert.That(loadedModule.Manifest.Version).IsEqualTo("1.2.3");
        await Assert.That(loadedModule.Manifest.Default.CSources).IsNotNull();
        await Assert.That(loadedModule.Manifest.Default.CSources.Values).IsNotNull();
        await Assert.That(loadedModule.Manifest.Default.CSources.Values.Count).IsEqualTo(1);
        await Assert.That(loadedModule.Manifest.Default.CSources.Values[0]).IsEqualTo("src/brieflz.c");
        await Assert.That(MockFileSystem.Normalize(loadedModule.ModuleDirectory)).IsEqualTo($"{qkRoot}/modules/brieflz");
    }

    [Test]
    public async Task RunPreBuildCommands_ShouldExpandTokens_AndCaptureRuntimeShorthands()
    {
        var mockFs = new MockFileSystem();
        var mockRunner = new MockProcessRunner();
        var shorthandEngine = new ShorthandEngine();

        shorthandEngine.SetToken("<PayloadDir>", "C:/App/Binaries");
        mockRunner.PlannedOutputs["python"] = (0, "ScrapedVersion=2.5.9-beta\n", "");

        var loadedModule = new LoadedModule
        {
            ModuleDirectory = "C:/UserProject/modules/my-scraper",
            ManifestPath = "C:/UserProject/modules/my-scraper/module.json",
            Manifest = new ModuleManifest
            {
                Id = "my-scraper",
                Version = "2.5.9-beta",
                Default = {
                    PreBuildCommands =
                    [
                        new()
                        {
                            Executable = "python",
                            Arguments = "script.py --target <PayloadDir>",
                            CaptureVariables = true
                        }
                    ]
                }
            }
        };

        var engine = new ModulesEngine("C:/QuarkitCore", mockFs, mockRunner);
        engine.RunPreBuildCommands(loadedModule, loadedModule.Manifest.Default, shorthandEngine);

        await Assert.That(mockRunner.History.Count).EqualTo(1);
        await Assert.That(mockRunner.History[0].Filename).IsEqualTo("python");
        await Assert.That(mockRunner.History[0].Arguments).IsEqualTo("script.py --target C:/App/Binaries");

        string expandedVersion = shorthandEngine.Expand("App version is <ScrapedVersion>");
        await Assert.That(expandedVersion).IsEqualTo("App version is 2.5.9-beta");
    }

    [Test]
    public async Task ResolveAndLoadModule_WithMalformedJson_ShouldThrowJsonException()
    {
        var mockFs = new MockFileSystem();
        var mockRunner = new MockProcessRunner();

        mockFs.CreateDirectory("C:/Project/modules/bad-json");
        mockFs.WriteAllText("C:/Project/modules/bad-json/module.json", "{ \"Name\": bad-module, "); // Name instead of Id, missing bracket "

        var engine = new ModulesEngine("C:/QuarkitCore", mockFs, mockRunner);

        var action = () => engine.FindAndLoadModule("./modules/bad-json", "C:/Project");
        await Assert.That(action).Throws<JsonException>();
    }

    [Test]
    public async Task ResolveAndLoadModule_WithMissingRequiredJson_ShouldThrowJsonException()
    {
        var mockFs = new MockFileSystem();
        var mockRunner = new MockProcessRunner();

        mockFs.CreateDirectory("C:/Project/modules/bad-json");
#pragma warning disable CS8625 // Cannot convert null literal to non-nullable reference type. - Disabled to simulate bad json.
        mockFs.WriteAllText("C:/Project/modules/bad-json/module.json", JsonSerializer.Serialize(
                new ModuleManifest()
                {
                    Id = null,
                    Version = null,
                    Default = {
                       CSources = new(["./src/bad-code.c"])
                    }
                },
                new JsonSerializerOptions() { DefaultIgnoreCondition = JsonIgnoreCondition.WhenWritingNull }
            )
        );
#pragma warning restore CS8625 // Cannot convert null literal to non-nullable reference type. - Disabled to simulate bad json.

        var engine = new ModulesEngine("C:/QuarkitCore", mockFs, mockRunner);

        var action = () => engine.FindAndLoadModule("./modules/bad-json", "C:/Project");
        await Assert.That(action).Throws<JsonException>();
    }

    [Test]
    public async Task RunPreBuildCommands_ShouldAnchorRelativeExecutablesToModuleDirectory()
    {
        var mockFs = new MockFileSystem();
        var mockRunner = new MockProcessRunner();
        var shorthandEngine = new ShorthandEngine();

        mockFs.WriteAllText("C:/Project/modules/local-tool/tools/compress.exe", "Mock compress.exe :)");
        var loadedModule = new LoadedModule
        {
            ModuleDirectory = "C:/Project/modules/local-tool",
            ManifestPath = "C:/Project/modules/local-tool/module.json",
            Manifest = new ModuleManifest
            {
                Id = "local-tool",
                Version = "1.0.0",
                Default = {
                    PreBuildCommands =
                    [
                        new() { Executable = "tools/compress.exe", Arguments = "--run" }
                    ]
                }
            }
        };

        var engine = new ModulesEngine("C:/QuarkitCore", mockFs, mockRunner);
        engine.RunPreBuildCommands(loadedModule, loadedModule.Manifest.Default, shorthandEngine);
        await Assert.That(MockFileSystem.Normalize(mockRunner.History[0].Filename)).IsEqualTo("C:/Project/modules/local-tool/tools/compress.exe");
    }

    [Test]
    public async Task RunPreBuildCommands_ShouldAnchorExplicitQKExecutablesToQKRootDirectory()
    {
        var mockFs = new MockFileSystem();
        var mockRunner = new MockProcessRunner();
        var shorthandEngine = new ShorthandEngine();
        shorthandEngine.SetToken("<QK>", "C:/QuarkitCore");

        var loadedModule = new LoadedModule
        {
            ModuleDirectory = "C:/Project/modules/local-tool",
            ManifestPath = "C:/Project/modules/local-tool/module.json",
            Manifest = new ModuleManifest
            {
                Id = "local-tool",
                Version = "1.0.0",
                Default = {
                    PreBuildCommands =
                    [
                        new() { Executable = "<QK>/tools/compress.exe", Arguments = "--run" } // Explicit <QK> root
                    ]
                }
            }
        };

        var engine = new ModulesEngine("C:/QuarkitCore", mockFs, mockRunner);
        engine.RunPreBuildCommands(loadedModule, loadedModule.Manifest.Default, shorthandEngine);
        await Assert.That(MockFileSystem.Normalize(mockRunner.History[0].Filename)).IsEqualTo("C:/QuarkitCore/tools/compress.exe");
    }

    [Test]
    public async Task RunPreBuildCommands_ErrorsWhenExplicitlyLocalExecutableIsMissing()
    {
        var mockFs = new MockFileSystem();
        var mockRunner = new MockProcessRunner();
        var shorthandEngine = new ShorthandEngine();

        var loadedModule = new LoadedModule
        {
            ModuleDirectory = "C:/Project/modules/local-tool",
            ManifestPath = "C:/Project/modules/local-tool/module.json",
            Manifest = new ModuleManifest
            {
                Id = "local-tool",
                Version = "1.0.0",
                Default = {
                    PreBuildCommands =
                    [
                        new() { Executable = "./tools/compress.exe", Arguments = "--run" }
                    ]
                }
            }
        };

        var engine = new ModulesEngine("C:/QuarkitCore", mockFs, mockRunner);
        var action = () => engine.RunPreBuildCommands(loadedModule, loadedModule.Manifest.Default, shorthandEngine);
        await Assert.That(action).Throws<InvalidDataException>();
    }

    [Test]
    public async Task RunPreBuildCommands_WhenScriptFails_ShouldSurfaceErrorAndHalt()
    {
        var mockFs = new MockFileSystem();
        var mockRunner = new MockProcessRunner();
        var shorthandEngine = new ShorthandEngine();

        // Mimics python error:
        mockRunner.PlannedOutputs["python crash.py"] = (1, "", "SyntaxError: invalid syntax at line 4"); // By default anything that isn't 0 fails.

        var loadedModule = new LoadedModule
        {
            ModuleDirectory = "C:/Project/modules/broken",
            ManifestPath = "C:/Project/modules/broken/module.json",
            Manifest = new ModuleManifest
            {
                Id = "broken-module",
                Version = "1.0.0",
                Default = {
                    PreBuildCommands = new List<ModuleCommand>
                    {
                        new() { Executable = "python", Arguments = "crash.py" }
                    }
                }
            }
        };

        var engine = new ModulesEngine("C:/QuarkitCore", mockFs, mockRunner);

        var action = () => engine.RunPreBuildCommands(loadedModule, loadedModule.Manifest.Default, shorthandEngine);
        var exception = await Assert.That(action).Throws<Exception>();
        await Assert.That(exception?.Message).Contains("SyntaxError: invalid syntax at line 4");
    }

    [Test]
    public async Task RunPreBuildCommands_WithSuccessCodes_DoesNotError()
    {
        var mockFs = new MockFileSystem();
        var mockRunner = new MockProcessRunner();
        var shorthandEngine = new ShorthandEngine();

        mockRunner.PlannedOutputs["python regular.py"] = (10, "Success! Code 10.", "");

        var loadedModule = new LoadedModule
        {
            ModuleDirectory = "C:/Project/modules/regular",
            ManifestPath = "C:/Project/modules/regular/module.json",
            Manifest = new ModuleManifest
            {
                Id = "regular-module",
                Version = "1.0.0",
                Default = {
                    PreBuildCommands = new List<ModuleCommand>
                    {
                        new() { Executable = "python", Arguments = "regular.py", SuccessCodes = [10] }
                    }
                }
            }
        };

        var engine = new ModulesEngine("C:/QuarkitCore", mockFs, mockRunner);
        var action = () => engine.RunPreBuildCommands(loadedModule, loadedModule.Manifest.Default, shorthandEngine);

        await Assert.That(action).ThrowsNothing();
    }

    [Test]
    public async Task RunPreBuildCommands_FailsWhenContainingADefinedStdErrPhrase()
    {
        var mockFs = new MockFileSystem();
        var mockRunner = new MockProcessRunner();
        var shorthandEngine = new ShorthandEngine();

        mockRunner.PlannedOutputs["python stderr-fail.py"] = (0, "", "CRITICAL ERROR");

        var loadedModule = new LoadedModule
        {
            ModuleDirectory = "C:/Project/modules/stderr-fail",
            ManifestPath = "C:/Project/modules/stderr-fail/module.json",
            Manifest = new ModuleManifest
            {
                Id = "stderr-fail-module",
                Version = "1.0.0",
                Default = {
                    PreBuildCommands = new List<ModuleCommand>
                    {
                        new() { Executable = "python", Arguments = "stderr-fail.py", FailIfOutputContains = "CRITICAL ERROR" }
                    }
                }
            }
        };

        var engine = new ModulesEngine("C:/QuarkitCore", mockFs, mockRunner);
        var action = () => engine.RunPreBuildCommands(loadedModule, loadedModule.Manifest.Default, shorthandEngine);

        await Assert.That(action).Throws<Exception>();

        mockRunner.PlannedOutputs["python stderr-fail.py"] = (0, "CRITICAL ERROR", ""); // Should work on regular STDOUT too.

        await Assert.That(action).Throws<Exception>();
    }

    [Test]
    public async Task ResolveModules_WithDeeplyNestedDependencies_PlacesThemInCorrectReverseOrder()
    {
        var mockFs = new MockFileSystem();
        var mockRunner = new MockProcessRunner();

        string projectPath = "C:/Project/";
        string quarkitCore = "C:/QuarkitCore";

        string moduleDir1 = Path.Combine(projectPath, "module-A");
        string moduleDir2 = Path.Combine(projectPath, "module-B");
        string moduleDir3 = Path.Combine(projectPath, "module-C");
        ModuleMockHelper.CreateMockLoadedModule("module-A", ["module-B"], moduleDir1, Path.Combine(moduleDir1, "module.json"), mockFs);
        ModuleMockHelper.CreateMockLoadedModule("module-B", ["module-C"], moduleDir2, Path.Combine(moduleDir2, "module.json"), mockFs);
        ModuleMockHelper.CreateMockLoadedModule("module-C", [], moduleDir3, Path.Combine(moduleDir3, "module.json"), mockFs);

        var engine = new ModulesEngine(quarkitCore, mockFs, mockRunner);
        var result = engine.ResolveModules(["module-A"], projectPath, new TargetKey(), new OverridesResolver());

        await Assert.That(result.Length).IsEqualTo(3);
        await Assert.That(result[0].Module.Manifest.Id).IsEqualTo("module-C");
        await Assert.That(result[1].Module.Manifest.Id).IsEqualTo("module-B");
        await Assert.That(result[2].Module.Manifest.Id).IsEqualTo("module-A");
    }

    [Test]
    public async Task ResolveModules_WithSharedDiamondDependency_PlacesDependencyOnceAtTheBeginning()
    {
        var mockFs = new MockFileSystem();
        var mockRunner = new MockProcessRunner();

        string projectPath = "C:/Project/";
        string quarkitCore = "C:/QuarkitCore";

        string moduleDir1 = Path.Combine(projectPath, "simple-extraction-preset"); // grouping module
        string moduleDir2 = Path.Combine(projectPath, "brieflz");
        string moduleDir3 = Path.Combine(projectPath, "quarkit-extraction");
        ModuleMockHelper.CreateMockLoadedModule("simple-extraction-preset", ["brieflz", "quarkit-extraction"], moduleDir1, Path.Combine(moduleDir1, "module.json"), mockFs);
        ModuleMockHelper.CreateMockLoadedModule("brieflz", ["quarkit-extraction"], moduleDir2, Path.Combine(moduleDir2, "module.json"), mockFs);
        ModuleMockHelper.CreateMockLoadedModule("quarkit-extraction", [], moduleDir3, Path.Combine(moduleDir3, "module.json"), mockFs);

        var engine = new ModulesEngine(quarkitCore, mockFs, mockRunner);
        var result = engine.ResolveModules(["simple-extraction-preset"], projectPath, new TargetKey(), new OverridesResolver());

        await Assert.That(result.Length).IsEqualTo(3);
        await Assert.That(result[0].Module.Manifest.Id).IsEqualTo("quarkit-extraction");
        await Assert.That(result[1].Module.Manifest.Id).IsEqualTo("brieflz");
        await Assert.That(result[2].Module.Manifest.Id).IsEqualTo("simple-extraction-preset");
    }

    [Test]
    public async Task ResolveModules_WhenUserManuallyDeclaresDependencyFirst_RespectsUserOrderAndSkipsDuplication()
    {
        var mockFs = new MockFileSystem();
        var mockRunner = new MockProcessRunner();

        string projectPath = "C:/Project/";
        string quarkitCore = "C:/QuarkitCore";

        string moduleDir1 = Path.Combine(projectPath, "brieflz");
        string moduleDir2 = Path.Combine(projectPath, "quarkit-extraction");

        ModuleMockHelper.CreateMockLoadedModule("brieflz", ["quarkit-extraction"], moduleDir1, Path.Combine(moduleDir1, "module.json"), mockFs);
        ModuleMockHelper.CreateMockLoadedModule("quarkit-extraction", [], moduleDir2, Path.Combine(moduleDir2, "module.json"), mockFs);

        var engine = new ModulesEngine(quarkitCore, mockFs, mockRunner);

        var result = engine.ResolveModules(["quarkit-extraction", "brieflz"], projectPath, new TargetKey(), new OverridesResolver());

        await Assert.That(result.Length).IsEqualTo(2);
        await Assert.That(result[0].Module.Manifest.Id).IsEqualTo("quarkit-extraction");
        await Assert.That(result[1].Module.Manifest.Id).IsEqualTo("brieflz");
    }

    [Test]
    public async Task ResolveModules_WithCircularDependency_GracefullyIncludesBothWithoutCrashing()
    {
        // It doesn't simply fail when a circular dependency is found, because
        // The modules might not actually depend on each other in a circular manner inside the code.
        // For example the code could be structured in a way where it has multiple "submodules"/headers
        // that don't cause circular dependency when used correctly.
        // It's still quite a weird approach but it gives the ability for modules to do stuff like that.

        var mockFs = new MockFileSystem();
        var mockRunner = new MockProcessRunner();

        string projectPath = "C:/Project/";
        string quarkitCore = "C:/QuarkitCore";

        string moduleDir1 = Path.Combine(projectPath, "module-A");
        string moduleDir2 = Path.Combine(projectPath, "module-B");

        ModuleMockHelper.CreateMockLoadedModule("module-A", ["module-B"], moduleDir1, Path.Combine(moduleDir1, "module.json"), mockFs);
        ModuleMockHelper.CreateMockLoadedModule("module-B", ["module-A"], moduleDir2, Path.Combine(moduleDir2, "module.json"), mockFs);

        var engine = new ModulesEngine(quarkitCore, mockFs, mockRunner);
        var result = engine.ResolveModules(["module-A"], projectPath, new TargetKey(), new OverridesResolver());

        await Assert.That(result.Length).IsEqualTo(2);
        await Assert.That(result[0].Module.Manifest.Id).EqualTo("module-B");
        await Assert.That(result[1].Module.Manifest.Id).EqualTo("module-A");
    }
}