using Quarkit.Core.Build;
using Quarkit.Models.Manifest;
using Quarkit.Models.Manifest.Modules;
using Quarkit.Tests.Mocks;

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
            ResolvedOptions = new InstallOptions(),
            ActiveModules = new List<LoadedModule>()
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
            ResolvedOptions = new InstallOptions(),
            ActiveModules = new List<LoadedModule>()
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
            ResolvedOptions = new InstallOptions
            {
                AppName = "TestSuiteApp",
                AdminRequired = true,
                DesktopShortcut = true,
                ExecutableToLaunch = "app.exe"
            },
            ActiveModules = new List<LoadedModule>
            {
                new()
                {
                    ModuleDirectory = "C:/Quarkit/modules/brieflz",
                    Manifest = new ModuleManifest
                    {
                        Id = "brieflz",
                        Version = "1.0.0",
                        CSources = new List<string> { "src/brieflz.c" },
                        CompilerFlags = new List<string> { "-DCONFIG_FAST_COMPRESSION" }
                    }
                }
            }
        };

        engine.Build(parameters);

        string args = processRunner.History[0].Arguments;

        // Verify Hardware Profile Macros
        await Assert.That(args).Contains("-DQUARKIT_OS_WINDOWS");
        await Assert.That(args).Contains("-DQUARKIT_OS_NAME=\"WINDOWS\"");
        await Assert.That(args).Contains("-DQUARKIT_ARCH_X86");
        await Assert.That(args).Contains("-DQUARKIT_ARCH_X86_64");
        await Assert.That(args).Contains("-DQUARKIT_BITNESS_64");

        // Verify Feature Macros
        await Assert.That(args).Contains("-DQUARKIT_REQUIRE_ADMIN");
        await Assert.That(args).Contains("-DQUARKIT_CREATE_DESKTOP_SHORTCUT");
        await Assert.That(args).Contains("-DQUARKIT_APP_NAME=\"TestSuiteApp\"");
        await Assert.That(args).Contains("-DQUARKIT_HAS_LAUNCH_TARGET");
        await Assert.That(args).Contains("-DQUARKIT_EXE_LAUNCH=\"app.exe\"");
        await Assert.That(args).Contains("-DQUARKIT_GENERATOR_VERSION=\"1.0.0-alpha.1\"");
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
            Manifest = new ModuleManifest
            {
                Id = "custom-logger",
                Version = "1.0.0",
                CSources = new List<string> { "src/log_core.c" }
            }
        };

        var moduleB = new LoadedModule
        {
            ModuleDirectory = "C:/Quarkit/modules/brieflz",
            Manifest = new ModuleManifest
            {
                Id = "brieflz",
                Version = "1.0.0",
                CSources = new List<string> { "src/lz.c" },
                CompilerFlags = new List<string> { "-DBRIEFLZ_MAX_LEVEL=5" }
            }
        };

        var parameters = new BuildParameters
        {
            CompilerName = "clang",
            CompilerType = CompilerType.Clang,
            OutputPath = "C:/Output/installer.exe",
            QuarkitRoot = "C:/Quarkit",
            Target = new TargetKey(OSSystem.Windows, Architecture.x86, Bitness.x64),
            ResolvedOptions = new InstallOptions(),
            ActiveModules = new List<LoadedModule> { moduleA, moduleB }
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
            ResolvedOptions = new InstallOptions(),
            ActiveModules = new List<LoadedModule>()
        };

        var action = () => engine.Build(parameters);

        var exception = await Assert.That(action).Throws<Exception>();
        await Assert.That(exception).IsNotNull();
        await Assert.That(exception.Message).Contains("Compilation failed via toolchain 'clang'");
        await Assert.That(exception.Message).Contains("fatal error: standard library header 'stddef.h' not found");
    }
}