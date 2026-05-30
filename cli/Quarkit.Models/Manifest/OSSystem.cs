using System.Text.Json.Serialization;

namespace Quarkit.Models.Manifest
{
    [JsonConverter(typeof(JsonStringEnumConverter))]
    public enum OSSystem : ushort
    {
        /// <summary>
        /// The operating system was not specified.
        /// </summary>
        None = 0,

        /// <summary>
        /// Linux system.
        /// </summary>
        Linux, 

        /// <summary>
        /// Windows system.
        /// </summary>
        Windows, 

        /// <summary>
        /// MacOS system.
        /// </summary>
        MacOS
    }
}
