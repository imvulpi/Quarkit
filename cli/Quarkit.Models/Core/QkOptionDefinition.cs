namespace Quarkit.Models.Core
{
    public class QkOptionDefinition
    {
        public required string Name { get; set; }
        public required QkType Type { get; set; }
        public string? DefaultValue { get; set; }
        public string? Values { get; set; }
    }
}
