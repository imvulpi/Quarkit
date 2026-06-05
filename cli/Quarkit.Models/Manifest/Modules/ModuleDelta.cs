using System.Text.Json.Serialization;

namespace Quarkit.Models.Manifest.Modules
{
    [JsonConverter(typeof(ModuleDeltaConverter))]
    public class ModuleDelta
    {
        public ModuleDelta() { }
        public ModuleDelta(List<string>? baseValues)
        {
            Values = baseValues;
        }

        public bool IsFullOverwrite => Values != null;

        public List<string>? Values { get; set; }
        public List<string>? Add { get; set; }
        public List<string>? Remove { get; set; }

        public static implicit operator ModuleDelta(List<string> list) =>
            new() { Values = list };
    }
}
