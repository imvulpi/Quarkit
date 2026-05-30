using System.Formats.Asn1;

namespace Quarkit.Models.Manifest
{
    public static class TargetStringExtensions
    {
        public static string AsString(this OSSystem system)
        {
            return system switch
            {
                OSSystem.Windows => "windows",
                OSSystem.Linux => "linux",
                OSSystem.MacOS => "macos",
                _ => "unknown"
            };
        }

        public static string AsString(this Architecture arch)
        {
            return arch switch
            {
                Architecture.x86 => "x86",
                Architecture.Arm => "arm",
                Architecture.RiscV => "riscv",
                _ => "unknown"
            };
        }

        public static string AsString(this Bitness bitness)
        {
            return bitness switch
            {
                Bitness.x32 => "32",
                Bitness.x64 => "64",
                _ => "unknown"
            };
        }

        public static string GetTriple(this TargetKey target)
        {
            return $"{target.System.AsString()}-{target.Arch.AsString()}_{target.Bit.AsString()}";
        }
    }
}
