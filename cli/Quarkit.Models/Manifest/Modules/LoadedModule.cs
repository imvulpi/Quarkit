namespace Quarkit.Models.Manifest.Modules;

public class LoadedModule
{
    /// <summary>
    /// The parsed structural manifest data.
    /// </summary>
    public required ModuleManifest Manifest { get; init; }

    /// <summary>
    /// The absolute path to this module's directory on the developer's machine.
    /// </summary>
    public required string ModuleDirectory { get; init; }

    /// <summary>
    /// The absolute path to this module's manifest file on the developer's machine.
    /// </summary>
    public required string ManifestPath { get; init; }
}