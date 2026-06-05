namespace Quarkit.Models.Manifest.Modules
{
    public class ModuleCommand
    {
        public required string Executable { get; set; }
        public string? Arguments { get; set; }
        public bool CaptureVariables { get; set; }
        public List<int>? SuccessCodes { get; set; }
        public bool? FailOnStdErr { get; set; }
        public string? FailIfOutputContains { get; set; }
        public Dictionary<string, string>? ShorthandOverrides { get; set; }
    }
}
