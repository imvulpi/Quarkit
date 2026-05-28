namespace Quarkit.Models.Manifest.Modules
{
    public class ModuleManifest
    {
        public required string Id { get; set; } 
        public required string Version { get; set; }
        public List<string>? Dependencies { get; set; }
        public List<string>? CSources { get; set; }
        public List<string>? CompilerFlags { get; set; }
        public List<string>? RequiredShorthands { get; set; }
        public List<ModuleCommand>? PreBuildCommands { get; set; }
    }

    public class ModuleCommand
    {
        public required string Executable { get; set; }
        public string? Arguments { get; set; }
        public bool CaptureVariables { get; set; }
    }
}
