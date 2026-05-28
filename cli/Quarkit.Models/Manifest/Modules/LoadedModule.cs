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
    /// Returns absolute file paths to all C sources so the BuildEngine can read them directly.
    /// </summary>
    public IEnumerable<string> GetAbsoluteCSources()
    {
        if (Manifest.CSources == null) return Enumerable.Empty<string>();

        return Manifest.CSources.Select(src =>
            Path.GetFullPath(Path.Combine(ModuleDirectory, src)));
    }
}