namespace Quarkit.Models.Manifest.Modules
{
    public class ResolvedModule
    {
        public required LoadedModule Module { get; set; }
        public required ModuleBlueprint Blueprint { get; set; }
    }
}
