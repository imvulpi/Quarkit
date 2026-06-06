using Quarkit.Core.Storage;
using Quarkit.Models.Core.Target;
using System.Runtime.InteropServices;
using Architecture = Quarkit.Models.Core.Target.Architecture;

namespace Quarkit.Core.Discovery
{
    public class DiscoveredPayload
    {
        public required TargetKey Target { get; init; }
        public required string FolderName { get; init; }
        public required string AbsolutePayloadPath { get; init; }
        public bool IsDirectory { get; init; }
    }

    public class PayloadDiscoveryEngine(IFileSystem fileSystem)
    {
        private readonly IFileSystem _fileSystem = fileSystem;

        /// <summary>
        /// Scans a base path for folders named after common RIDs and resolves their final payload locations.
        /// </summary>
        /// <param name="autoDiscoveryBasePath">The top-level folder containing compilation output (e.g., "Release/")</param>
        /// <param name="payloadSuffix">The path suffix to append to the found RID folder (e.g., "publish/myapp.exe")</param>
        public List<DiscoveredPayload> DiscoverPayloads(string autoDiscoveryBasePath, string payloadSuffix)
        {
            var results = new List<DiscoveredPayload>();

            if (!_fileSystem.DirectoryExists(autoDiscoveryBasePath))
            {
                return results;
            }

            IEnumerable<string> subDirectories = _fileSystem.GetDirectories(autoDiscoveryBasePath);
            foreach (string dirPath in subDirectories)
            {
                string folderName = Path.GetFileName(dirPath);
                TargetKey? parsedTarget = TryParseRid(folderName);
                if (parsedTarget.Equals(null)) continue;

                string fullCombinedPath = Path.GetFullPath(Path.Combine(dirPath, payloadSuffix));
                bool fileExists = _fileSystem.FileExists(fullCombinedPath);
                bool dirExists = _fileSystem.DirectoryExists(fullCombinedPath);

                if (fileExists || dirExists)
                {
                    results.Add(new DiscoveredPayload
                    {
                        Target = parsedTarget.Value,
                        FolderName = folderName,
                        AbsolutePayloadPath = fullCombinedPath,
                        IsDirectory = dirExists && !fileExists
                    });
                }
            }

            return results;
        }

        /// <summary>
        /// Parses a potential RID string (e.g., "win-x64", "linux-arm") into a TargetKey.
        /// </summary>
        public static TargetKey? TryParseRid(string potentialRid)
        {
            if (string.IsNullOrWhiteSpace(potentialRid)) return null;
            potentialRid = potentialRid.Trim().ToLower();

            OSSystem system = OSSystem.None;
            if (potentialRid.Contains("win")) system = OSSystem.Windows;
            else if (potentialRid.Contains("lin")) system = OSSystem.Linux;
            else if (potentialRid.Contains("mac") || potentialRid.Contains("osx")) system = OSSystem.MacOS;

            Architecture arch = Architecture.None;
            Bitness bit = Bitness.x32;
            if (potentialRid.Contains("64")) bit = Bitness.x64;

            if (potentialRid.Contains("x86") || potentialRid.Contains("amd")) return new(system, Architecture.x86, bit);
            else if(potentialRid.Contains("arm") || potentialRid.Contains("aarch")) return new(system, Architecture.Arm, bit); 
            else if (potentialRid.Contains("riscv") || potentialRid.Contains("mips")) return new(system, Architecture.RiscV, bit);
            else if (arch == Architecture.None && potentialRid.Contains("x64")) return new(system, Architecture.x86, Bitness.x64);

            return null;
        }

        /// <summary>
        /// Introspects the local machine environment to auto-generate a native TargetKey baseline.
        /// </summary>
        public static TargetKey DiscoverHostTarget()
        {
            OSSystem system = OSSystem.None;
            if (RuntimeInformation.IsOSPlatform(OSPlatform.Windows)) system = OSSystem.Windows;
            else if (RuntimeInformation.IsOSPlatform(OSPlatform.Linux)) system = OSSystem.Linux;
            else if (RuntimeInformation.IsOSPlatform(OSPlatform.OSX)) system = OSSystem.MacOS;
            else throw new PlatformNotSupportedException("Quarkit build environment must be initialized on Windows, Linux, or macOS.");

            Architecture arch = RuntimeInformation.OSArchitecture switch
            {
                System.Runtime.InteropServices.Architecture.X64 => Architecture.x86,
                System.Runtime.InteropServices.Architecture.X86 => Architecture.x86,
                System.Runtime.InteropServices.Architecture.Arm64 => Architecture.Arm,
                System.Runtime.InteropServices.Architecture.Arm => Architecture.Arm,
                _ => throw new PlatformNotSupportedException($"Host CPU architecture '{RuntimeInformation.OSArchitecture}' is not supported by Quarkit Cli.")
            };

            Bitness bit = Environment.Is64BitOperatingSystem ? Bitness.x64 : Bitness.x32;

            return new TargetKey(system, arch, bit);
        }
    }
}
