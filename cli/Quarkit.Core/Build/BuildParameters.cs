using Quarkit.Models.Manifest;
using Quarkit.Models.Manifest.Modules;

namespace Quarkit.Core.Build
{
    public class BuildParameters
    {
        public required TargetKey Target { get; init; }
        public required string OutputPath { get; init; }
        public required string QuarkitRoot { get; init; }

        // Flexible Compiler Settings
        public string CompilerName { get; set; } = "clang";
        public CompilerType CompilerType { get; init; } = CompilerType.Clang;

        // Direct domain engine objects
        public required InstallOptions ResolvedOptions { get; init; }
        public required List<LoadedModule> ActiveModules { get; init; } = [];
    }
}
