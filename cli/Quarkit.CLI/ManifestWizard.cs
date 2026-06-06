namespace Quarkit.CLI
{
    using Quarkit.Models.Manifest.Installer;
    using System;
    using System.Collections.Generic;
    using System.IO;
    using System.Text.Json;
    using System.Text.Json.Serialization;

    public static class ManifestWizard
    {
        private readonly static JsonSerializerOptions jsonOptions = new()
        {
            WriteIndented = true,
            DefaultIgnoreCondition = JsonIgnoreCondition.WhenWritingNull
        };

        public static void Run()
        {
            Console.Clear();
            Console.ForegroundColor = ConsoleColor.Cyan;
            Console.WriteLine("========================================");
            Console.WriteLine("    Quarkit Installer Manifest Setup    ");
            Console.WriteLine("========================================");
            Console.ResetColor();

            string projectName = Prompt("Enter the software name (e.g., MyApp)", allowEmpty: false);

            Console.WriteLine("\nChoose install scope:");
            Console.WriteLine(" [1] Machine (Installs for all users, usually requires Admin)");
            Console.WriteLine(" [2] User    (Installs per-user, usually no Admin required)");
            string scopeChoice = Prompt("Select option [1-2]", "1");
            string installScope = scopeChoice == "2" ? "user" : "machine";

            Console.WriteLine("\n--- AutoDiscovery Configuration ---");
            Console.WriteLine("Quarkit will scan this path for system-architecture folders (e.g., win-x64, linux-arm64).");
            string basePath = Prompt("Enter base binaries path (e.g., DevProject/Releases/)", allowEmpty: false);
            string suffix = Prompt("Enter target file/folder suffix (e.g., DevProject.exe or leave empty if none)");
            string programName = Prompt("\nEnter the main executable name to launch after install (e.g., bin/myapp.exe)", allowEmpty: false);

            Console.WriteLine("\n[i] Scanning for supported targets...");
            List<string> discoveredTargets = DiscoverTargets(basePath);

            if (discoveredTargets.Count == 0)
            {
                Console.ForegroundColor = ConsoleColor.Yellow;
                Console.WriteLine("[!] Warning: No standard target triples (like win-x64, linux-x64) were automatically found in that path.");
                Console.WriteLine("    This is fine as long as your framework supports the target triples when building the installers.");
                Console.ResetColor();
                
                // Some default targets:
                discoveredTargets.AddRange(["windows-x64", "linux-x64"]);
            }
            else
            {
                Console.ForegroundColor = ConsoleColor.Green;
                Console.WriteLine($"[✓] Found {discoveredTargets.Count} target(s): {string.Join(", ", discoveredTargets)}");
                Console.ResetColor();
            }

            InstallerManifest manifest = new()
            {
                AutoDiscovery = new()
                {
                    TargetRootDirectory = basePath,
                    TargetPayloadSuffix = suffix,
                },
                Default = new()
                {
                    AdminRequired = (installScope == "machine"),
                    AppName = projectName,
                    ExecutableToLaunch = programName,
                    TargetPath = installScope == "machine" ? "%PROGRAMFILES%/" + projectName : "%LOCALAPPDATA%/" + projectName,
                    DesktopShortcut = true,
                    StartShortcut = true
                },
                Overrides = []
            };

            string fileName = "quarkit.json";
            string jsonString = JsonSerializer.Serialize(manifest, jsonOptions);

            File.WriteAllText(fileName, jsonString);

            Console.ForegroundColor = ConsoleColor.Green;
            Console.WriteLine($"\n[✓] Successfully created '{fileName}'!");
            Console.ResetColor();
        }

        private static string Prompt(string message, string defaultValue = "", bool allowEmpty = true)
        {
            while (true)
            {
                Console.Write($"{message}{(string.IsNullOrEmpty(defaultValue) ? "" : $" [{defaultValue}]")}: ");
                string? input = Console.ReadLine()?.Trim();

                if (string.IsNullOrEmpty(input))
                {
                    if (!string.IsNullOrEmpty(defaultValue)) return defaultValue;
                    if (allowEmpty) return string.Empty;

                    Console.ForegroundColor = ConsoleColor.Red;
                    Console.WriteLine("This field cannot be left blank.");
                    Console.ResetColor();
                    continue;
                }
                return input;
            }
        }

        private static List<string> DiscoverTargets(string basePath)
        {
            var targets = new List<string>();
            if (!Directory.Exists(basePath)) return targets;

            // Common runtime identifiers or triples to scan for
            var knownPatterns = new HashSet<string>(StringComparer.OrdinalIgnoreCase)
            {
                "win-x64", "win-x86", "win-arm64", "windows-x64", "windows-arm64",
                "linux-x64", "linux-arm64", "osx-x64", "osx-arm64"
            };

            try
            {
                var directories = Directory.GetDirectories(basePath);
                foreach (var dir in directories)
                {
                    string dirName = Path.GetFileName(dir).ToLowerInvariant();
                    if (knownPatterns.Contains(dirName))
                    {
                        targets.Add(dirName);
                    }
                }
            }
            catch (Exception)
            {
                // Fail silently
            }

            return targets;
        }
    }
}
