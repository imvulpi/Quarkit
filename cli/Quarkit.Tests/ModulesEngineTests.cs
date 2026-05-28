using Quarkit.Core.Modules;
using Quarkit.Core.Shorthand;
using Quarkit.Models.Manifest.Modules;
using Quarkit.Tests.Mocks;
using System.Data;
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

    [Test]
    public async Task ResolveAndLoadModule_WithMalformedJson_ShouldThrowJsonException()
    {
        var mockFs = new MockFileSystem();
        var mockRunner = new MockProcessRunner();

        mockFs.CreateDirectory("C:/Project/modules/bad-json");
        mockFs.WriteAllText("C:/Project/modules/bad-json/module.json",  "{ \"Name\": bad-module, "); // Name instead of Id, missing bracket "

        var engine = new ModulesEngine("C:/QuarkitCore", mockFs, mockRunner);

        var action = () => engine.ResolveAndLoadModule("./modules/bad-json", "C:/Project");
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
                new ModuleManifest() { 
                    Id = null, 
                    Version = null, 
                    CSources = ["./src/bad-code.c"] 
                },
                new JsonSerializerOptions() { DefaultIgnoreCondition = JsonIgnoreCondition.WhenWritingNull }
            )
        );
#pragma warning restore CS8625 // Cannot convert null literal to non-nullable reference type. - Disabled to simulate bad json.

        var engine = new ModulesEngine("C:/QuarkitCore", mockFs, mockRunner);

        var action = () => engine.ResolveAndLoadModule("./modules/bad-json", "C:/Project");
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
            Manifest = new ModuleManifest
            {
                Id = "local-tool",
                Version = "1.0.0",
                PreBuildCommands =
                [
                    new() { Executable = "tools/compress.exe", Arguments = "--run" }
                ]
            }
        };

        var engine = new ModulesEngine("C:/QuarkitCore", mockFs, mockRunner);
        engine.RunPreBuildCommands(loadedModule, shorthandEngine);
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
            Manifest = new ModuleManifest
            {
                Id = "local-tool",
                Version = "1.0.0",
                PreBuildCommands =
                [
                    new() { Executable = "<QK>/tools/compress.exe", Arguments = "--run" } // Explicit <QK> root
                ]
            }
        };

        var engine = new ModulesEngine("C:/QuarkitCore", mockFs, mockRunner);
        engine.RunPreBuildCommands(loadedModule, shorthandEngine);
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
            Manifest = new ModuleManifest
            {
                Id = "local-tool",
                Version = "1.0.0",
                PreBuildCommands =
                [
                    new() { Executable = "./tools/compress.exe", Arguments = "--run" }
                ]
            }
        };

        var engine = new ModulesEngine("C:/QuarkitCore", mockFs, mockRunner);
        var action = () => engine.RunPreBuildCommands(loadedModule, shorthandEngine);
        await Assert.That(action).Throws<InvalidDataException>();
    }
}