using Quarkit.Models.Manifest;
using Quarkit.Models.Manifest.Installer;
using Quarkit.Models.Manifest.Modules;

namespace Quarkit.Core.Build
{
    public class BuildParameters
    {
        public required TargetKey Target { get; init; }
        public required string OutputPath { get; init; }
        public required string QuarkitRoot { get; init; }

        // Payload
        public string PayloadPath { get; init; } = string.Empty;
        public string PayloadName { get; init; } = "payload";

        // Flexible Compiler Settings
        public string CompilerName { get; set; } = "clang";
        public CompilerType CompilerType { get; init; } = CompilerType.Clang;

        // Direct domain engine objects
        public required InstallerBlueprint ResolvedOptions { get; init; }
        public required List<ResolvedModule> ResolvedModules { get; init; } = [];
    }
}
