using Quarkit.Core.Build;
using Quarkit.Models.Core.Target;
using Quarkit.Models.Manifest.Installer;
using Quarkit.Models.Manifest.Modules;
using Quarkit.Tests.Mocks;

namespace Quarkit.Tests;

public class BuildEngineTests
{
    [Test]
    public async Task Build_WithClangCompiler_ShouldAppendTargetFlagsAndOptimizationArguments()
    {
        MockFileSystem fileSystem = new MockFileSystem();
        MockProcessRunner processRunner = new MockProcessRunner();
        BuildEngine engine = new BuildEngine(fileSystem, processRunner);

        var parameters = new BuildParameters
        {
            CompilerName = "clang",
            CompilerType = CompilerType.Clang,
            OutputPath = "C:/Output/bin/installer.exe",
            QuarkitRoot = "C:/Quarkit",
            Target = new TargetKey(OSSystem.Windows, Architecture.x86, Bitness.x64),
            ResolvedOptions = new InstallerBlueprint(),
            ResolvedModules = []
        };

        engine.Build(parameters);

        // Verify auto-createation of missing directories
        await Assert.That(fileSystem.DirectoryExists("C:/Output/bin")).IsTrue();

        // Verify compiler and target
        var cmd = processRunner.History[0];
        await Assert.That(cmd.Filename).IsEqualTo("clang");
        await Assert.That(cmd.Arguments).Contains("-target x86_64-w64-windows-gnu");

        // Verify optimizations
        await Assert.That(cmd.Arguments).Contains("-Oz");
        await Assert.That(cmd.Arguments).Contains("-nostdlib");
        await Assert.That(cmd.Arguments).Contains("-fuse-ld=lld");
        await Assert.That(cmd.Arguments).Contains("-lkernel32");
    }

    [Test]
    public async Task Build_WithGccCompiler_ShouldMutateCompilerExecutableNameAndOmitTargetFlag()
    {
        MockFileSystem fileSystem = new MockFileSystem();
        MockProcessRunner processRunner = new MockProcessRunner();
        BuildEngine engine = new BuildEngine(fileSystem, processRunner);

        var parameters = new BuildParameters
        {
            CompilerName = "gcc", // The engine will change this to the cross-compiler name
            CompilerType = CompilerType.Gcc,
            OutputPath = "C:/Output/installer.exe",
            QuarkitRoot = "C:/Quarkit",
            Target = new TargetKey(OSSystem.Linux, Architecture.Arm, Bitness.x32),
            ResolvedOptions = new InstallerBlueprint(),
            ResolvedModules = []
        };

        engine.Build(parameters);

        // Verify binary name change to cross-linux-gnueabihf-gcc toolchain with no "-target" flag leaks
        var cmd = processRunner.History[0];
        await Assert.That(cmd.Filename).IsEqualTo("arm-linux-gnueabihf-gcc");
        await Assert.That(cmd.Arguments).DoesNotContain("-target");
    }

    [Test]
    public async Task Build_ShouldCorrectlyMapGlobalAndActiveModuleMacroDefinitions()
    {
        MockFileSystem fileSystem = new MockFileSystem();
        MockProcessRunner processRunner = new MockProcessRunner();
        BuildEngine engine = new BuildEngine(fileSystem, processRunner);

        var parameters = new BuildParameters
        {
            CompilerName = "clang",
            CompilerType = CompilerType.Clang,
            OutputPath = "C:/Output/installer.exe",
            QuarkitRoot = "C:/Quarkit",
            Target = new TargetKey(OSSystem.Windows, Architecture.x86, Bitness.x64),
            ResolvedOptions = new InstallerBlueprint
            {
                AppName = "TestSuiteApp",
                AdminRequired = true,
                DesktopShortcut = true,
                ExecutableToLaunch = "app.exe"
            },
            ResolvedModules = ModuleMockHelper.GetResolvedModulesFromDefaults(
                new List<LoadedModule>
                {
                    new()
                    {
                        ModuleDirectory = "C:/Quarkit/modules/brieflz",
                        ManifestPath = "C:/Quarkit/modules/brieflz/manifest.json",
                        Manifest = new ModuleManifest
                        {
                            Id = "brieflz",
                            Version = "1.0.0",
                            Default = {
                                CSources = new List<string> { "src/brieflz.c" },
                                CompilerFlags = new List<string> { "-DCONFIG_FAST_COMPRESSION" }
                            }
                        }
                    }
                }).ToArray()
        };
        fileSystem.WriteAllText("C:/Quarkit/modules/brieflz/src/brieflz.c", "// This is a module test.");

        engine.Build(parameters);

        string args = processRunner.History[0].Arguments;

        // Verify Hardware Profile Macros
        await Assert.That(args).Contains("-DQUARKIT_OS_WINDOWS");
        await Assert.That(args).Contains($"-DQUARKIT_OS_NAME={BuildEngine.ESC_STRING}WINDOWS{BuildEngine.ESC_STRING}");
        await Assert.That(args).Contains("-DQUARKIT_ARCH_X86");
        await Assert.That(args).Contains("-DQUARKIT_ARCH_X86_64");
        await Assert.That(args).Contains("-DQUARKIT_BITNESS_64");

        // Verify Feature Macros
        await Assert.That(args).Contains("-DQUARKIT_REQUIRE_ADMIN");
        await Assert.That(args).Contains("-DQUARKIT_CREATE_DESKTOP_SHORTCUT");
        await Assert.That(args).Contains($"-DQUARKIT_APP_NAME={BuildEngine.ESC_STRING}TestSuiteApp{BuildEngine.ESC_STRING}");
        await Assert.That(args).Contains("-DQUARKIT_HAS_LAUNCH_TARGET");
        await Assert.That(args).Contains($"-DQUARKIT_EXE_LAUNCH={BuildEngine.ESC_STRING}app.exe{BuildEngine.ESC_STRING}");
        await Assert.That(args).Contains($"-DQUARKIT_GENERATOR_VERSION={BuildEngine.ESC_STRING}1.0.0-alpha.1{BuildEngine.ESC_STRING}");
    }

    [Test]
    public async Task Build_WithActiveModules_ShouldStitchIncludesInitsAndCustomFlags()
    {
        MockFileSystem fileSystem = new MockFileSystem();
        MockProcessRunner processRunner = new MockProcessRunner();
        BuildEngine engine = new BuildEngine(fileSystem, processRunner);

        var moduleA = new LoadedModule
        {
            ModuleDirectory = "C:/Quarkit/modules/logger",
            ManifestPath = "C:/Quarkit/modules/logger/manifest.json",
            Manifest = new ModuleManifest
            {
                Id = "custom-logger",
                Version = "1.0.0",
                Default = {
                    CSources = new List<string> { "src/log_core.c" }
                }
            }
        };

        fileSystem.WriteAllText("C:/Quarkit/modules/logger/src/log_core.c", "void quarkit_custom_logger_init() { // mock test }");

        var moduleB = new LoadedModule
        {
            ModuleDirectory = "C:/Quarkit/modules/brieflz",
            ManifestPath = "C:/Quarkit/modules/logger/manifest.json",
            Manifest = new ModuleManifest
            {
                Id = "brieflz",
                Version = "1.0.0",
                Default = {
                    CSources = new List<string> { "src/lz.c" },
                    CompilerFlags = new List<string> { "-DBRIEFLZ_MAX_LEVEL=5" }
                }
            }
        };

        fileSystem.WriteAllText("C:/Quarkit/modules/brieflz/src/lz.c", "void quarkit_brieflz_init() { // mock test }");

        var parameters = new BuildParameters
        {
            CompilerName = "clang",
            CompilerType = CompilerType.Clang,
            OutputPath = "C:/Output/installer.exe",
            QuarkitRoot = "C:/Quarkit",
            Target = new TargetKey(OSSystem.Windows, Architecture.x86, Bitness.x64),
            ResolvedOptions = new InstallerBlueprint(),
            ResolvedModules = ModuleMockHelper.GetResolvedModulesFromDefaults([moduleA, moduleB]).ToArray()
        };

        engine.Build(parameters);

        string args = processRunner.History[0].Arguments;
        args = MockFileSystem.Normalize(args); // Normalize to our mock system.

        // Verify multiple header search flags (-I)
        await Assert.That(args).Contains("-I\"C:/Quarkit/modules/logger/include\"");
        await Assert.That(args).Contains("-I\"C:/Quarkit/modules/brieflz/include\"");

        // Verify module initialization calls are sequenced correctly inside the preprocessor definition hook
        await Assert.That(args).Contains("-DQUARKIT_MODULE_INITS=\"quarkit_custom_logger_init(); quarkit_brieflz_init();\"");

        // Verify module manifest internal custom compiler flags were appended 
        await Assert.That(args).Contains("-DBRIEFLZ_MAX_LEVEL=5");

        // Verify relative C source targets resolution logic
        await Assert.That(args).Contains("\"C:/Quarkit/modules/logger/src/log_core.c\"");
        await Assert.That(args).Contains("\"C:/Quarkit/modules/brieflz/src/lz.c\"");
    }

    [Test]
    public async Task Build_WhenToolchainProcessFails_ShouldThrowExceptionSurfacingErrorMessage()
    {
        MockFileSystem fileSystem = new MockFileSystem();
        MockProcessRunner processRunner = new MockProcessRunner();
        BuildEngine engine = new BuildEngine(fileSystem, processRunner);

        processRunner.PlannedOutputs["clang"] = (1, "", "fatal error: standard library header 'stddef.h' not found");

        var parameters = new BuildParameters
        {
            CompilerName = "clang",
            CompilerType = CompilerType.Clang,
            OutputPath = "C:/Output/installer.exe",
            QuarkitRoot = "C:/Quarkit",
            Target = new TargetKey(OSSystem.Windows, Architecture.x86, Bitness.x64),
            ResolvedOptions = new InstallerBlueprint(),
            ResolvedModules = []
        };

        var action = () => engine.Build(parameters);

        var exception = await Assert.That(action).Throws<Exception>();
        await Assert.That(exception).IsNotNull();
        await Assert.That(exception.Message).Contains("Compilation failed via toolchain 'clang'");
        await Assert.That(exception.Message).Contains("fatal error: standard library header 'stddef.h' not found");
    }
}