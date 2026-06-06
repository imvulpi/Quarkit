using System.Text.Json.Serialization;

namespace Quarkit.Models.Core.Target
{
    [JsonConverter(typeof(JsonStringEnumConverter))]
    public enum Bitness : byte
    {
        /// <summary>
        /// The bitness of the system was not specified.
        /// </summary>
        None = 0,

        /// <summary>
        /// 32 bit systems.
        /// </summary>
        x32,

        /// <summary>
        /// 64 bit systems.
        /// </summary>
        x64
    }
}
