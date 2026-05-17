using System.Text.Json.Serialization;

namespace Quarkit.Models.Manifest
{
    [JsonConverter(typeof(JsonStringEnumConverter))]
    public enum Architecture : byte
    {
        /// <summary>
        /// The cpu architecture was not specified.
        /// </summary>
        None = 0,

        /// <summary>
        /// x86 Cpu Architecture (x86_32bit and x86_64 bit).
        /// </summary>
        x86,

        /// <summary>
        /// Arm Cpu Architecture.
        /// </summary>
        Arm,

        /// <summary>
        /// RiscV Cpu Architecture.
        /// </summary>
        RiscV
    }
}
