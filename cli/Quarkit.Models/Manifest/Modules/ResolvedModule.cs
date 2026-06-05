namespace Quarkit.Models.Manifest.Modules
{
    public class ResolvedModule
    {
        public required LoadedModule Module { get; set; }
        public required ModuleBlueprint Blueprint { get; set; }

        /// <summary>
        /// Returns absolute file paths to all C sources so the BuildEngine can read them directly.
        /// </summary>
        public IEnumerable<string> GetAbsoluteCSources()
        {
            if (Blueprint.CSources == null || Blueprint.CSources.Values == null) return [];

            return Blueprint.CSources.Values.Select(src =>
                Path.GetFullPath(Path.Combine(Module.ModuleDirectory, src)));
        }
    }
}
