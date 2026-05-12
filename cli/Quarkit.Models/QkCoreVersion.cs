using System.Runtime.InteropServices;

namespace Quarkit.Models
{
    /// <summary>
    /// A memory-aligned, 10-byte fixed-size header representing a software version.
    /// </summary>
    /// <remarks>
    /// IMPORTANT: This structure is designed to be "reinterpretable." It is often followed 
    /// by a contiguous block of memory of length <see cref="ExtensionSize"/>. 
    /// <para>
    /// <b>Binary Layout:</b>
    /// <list type="bullet">
    /// <item>Bytes 0-5: Major, Minor, Patch (ushorts)</item>
    /// <item>Byte 6: Stage (byte-enum)</item>
    /// <item>Bytes 7-8: Revision (ushort)</item>
    /// <item>Byte 9: ExtensionSize (byte)</item>
    /// </list>
    /// </para>
    /// </remarks>
    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    public struct QkCoreVersion
    {
        /// <summary> 
        /// Major product generation or breaking change identifier. 
        /// </summary>
        public ushort Major;

        /// <summary> 
        /// Feature-level updates. 
        /// </summary>
        public ushort Minor;

        /// <summary> 
        /// Backward-compatible bug fixes and patches. 
        /// </summary>
        public ushort Patch;

        /// <summary> 
        /// The stability stage of the build. Meaning is defined by the organization. 
        /// </summary>
        public QkVersionStage Stage;

        /// <summary> 
        /// Monotonic build counter or minor revision within the current stage. 
        /// </summary>
        public ushort Revision;

        /// <summary> 
        /// The size (in bytes) of the optional metadata extension following this struct. 
        /// Used for storing Git SHAs, timestamps, or build hashes.
        /// </summary>
        public byte ExtensionSize;

        /// <summary>
        /// Returns the standard shorthand: v{Major}.{Minor}.{Patch}-{Stage}.{Revision}
        /// </summary>
        public override string ToString() => $"{Major}.{Minor}.{Patch}-{Stage}.{Revision}";
    }
}
