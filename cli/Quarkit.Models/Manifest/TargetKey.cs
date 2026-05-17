using System.Runtime.InteropServices;
using System.Runtime.Serialization;
using System.Text.Json.Serialization;

namespace Quarkit.Models.Manifest
{
    [StructLayout(LayoutKind.Explicit, Size = 4)]
    public readonly struct TargetKey : IEquatable<TargetKey>
    {
        // The underlying 4B value of the key.
        [JsonIgnore]
        [IgnoreDataMember]
        [FieldOffset(0)] public readonly uint Value;

        // Internal hardware-aligned fields
        [FieldOffset(0)] private readonly OSSystem _system;
        [FieldOffset(2)] private readonly Architecture _arch;
        [FieldOffset(3)] private readonly Bitness _bit;

        public OSSystem System => _system;
        public Architecture Arch => _arch;
        public Bitness Bit => _bit;

        [JsonConstructor]
        public TargetKey(OSSystem system = OSSystem.None, Architecture arch = Architecture.None, Bitness bit = Bitness.None)
        {
            _system = system;
            _arch = arch;
            _bit = bit;
        }

        // Direct performance overrides for Dictionary optimization
        public bool Equals(TargetKey other) => Value == other.Value;
        public override bool Equals(object? obj) => obj is TargetKey other && Equals(other);
        public override int GetHashCode() => (int)Value; // Zero-overhead hashing!

        public static bool operator ==(TargetKey left, TargetKey right) => left.Value == right.Value;
        public static bool operator !=(TargetKey left, TargetKey right) => left.Value != right.Value;
    }
}
