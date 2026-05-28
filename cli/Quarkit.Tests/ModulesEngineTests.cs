using Quarkit.Core.Modules;
using Quarkit.Core.Shorthand;
using Quarkit.Models.Manifest.Modules;
using Quarkit.Tests.Mocks;
using System.Text.Json;

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
                CSources = ["src/brieflz.c"]
            })
        );

        var engine = new ModulesEngine(qkRoot, mockFs, mockRunner);
        var loadedModule = engine.ResolveAndLoadModule("brieflz", manifestDir);

        await Assert.That(loadedModule.Manifest.Id).IsEqualTo("brieflz");
        await Assert.That(loadedModule.Manifest.Version).IsEqualTo("1.2.3");
        await Assert.That(loadedModule.Manifest.CSources).IsNotNull();
        await Assert.That(loadedModule.Manifest.CSources?.Count).IsEqualTo(1);
        await Assert.That(loadedModule.Manifest.CSources[0]).IsEqualTo("src/brieflz.c");
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
            Manifest = new ModuleManifest
            {
                Id = "my-scraper",
                Version = "2.5.9-beta",
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
        };

        var engine = new ModulesEngine("C:/QuarkitCore", mockFs, mockRunner);
        engine.RunPreBuildCommands(loadedModule, shorthandEngine);

        await Assert.That(mockRunner.History.Count).EqualTo(1);
        await Assert.That(mockRunner.History[0].Filename).IsEqualTo("python");
        await Assert.That(mockRunner.History[0].Arguments).IsEqualTo("script.py --target C:/App/Binaries");

        string expandedVersion = shorthandEngine.Expand("App version is <ScrapedVersion>");
        await Assert.That(expandedVersion).IsEqualTo("App version is 2.5.9-beta");
    }
}