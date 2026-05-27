namespace Quarkit.Models.Manifest
{
    public class InstallerCreatorOptions
    {
        /// <summary>
        /// The name or path of the compiler executable to target. (e.g. "clang", "gcc" ...)
        /// </summary>
        public string CompilerName { get; set; } = "clang";
    }
}
