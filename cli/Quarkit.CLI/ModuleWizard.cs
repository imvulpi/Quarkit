using System.Text.Encodings.Web;
using System.Text.Json;
using System.Text.Json.Serialization;

namespace Quarkit.CLI;

public static class ModuleWizard
{
    public const string MODULE_FILENAME = "module.json";

    public static void Run()
    {
        Console.Clear();
        Console.ForegroundColor = ConsoleColor.Magenta;
        Console.WriteLine("========================================");
        Console.WriteLine("       Quarkit Module Init Wizard       ");
        Console.WriteLine("========================================");
        Console.ResetColor();

        string moduleId = Prompt("Enter unique Module ID (e.g., app-patcher, brieflz)", allowEmpty: false)
                            .ToLowerInvariant()
                            .Replace(" ", "-"); // Enforce clean directory/ID naming

        if (Directory.Exists(moduleId))
        {
            string shouldOverrite = Prompt("Module with that name already exists! Should I Overwrite? (y/n)", "n");
            if (shouldOverrite.Equals("n", StringComparison.CurrentCultureIgnoreCase))
            {
                return;
            }
            else
            {
                Directory.Delete(moduleId, true);
            }
        }

        Console.WriteLine("\nWhat type of module are you creating?");
        Console.WriteLine(" [1] Pure Script / Pipeline Runner (Runs commands, no C installer code)");
        Console.WriteLine(" [2] C Runtime Component           (Injects C code/flags into the installer)");
        Console.WriteLine(" [3] Hybrid                        (Runs script commands AND injects C code)");
        string typeChoice = Prompt("Select option [1-3]", "1");

        List<string>? cSources = null;
        List<string>? compilerFlags = null;
        List<object>? preBuildCommands = null;

        if (typeChoice == "1" || typeChoice == "3") // Pure or Hybrid
        {
            Console.WriteLine("\n--- Script Configuration ---");
            string scriptLang = Prompt("What script/executable will this run? (e.g., python, bash, node)", "python");
            string scriptFile = Prompt("Enter the script file name", scriptLang == "python" ? $"run_{moduleId.Replace("-", "_")}.py" : $"run_{moduleId.Replace("-", "_")}.sh");

            preBuildCommands = new List<object>
            {
                new
                {
                    Executable = scriptLang,
                    Arguments = $"{scriptFile} --dir <ModuleDir> --payload <PayloadDir>",
                    CaptureVariables = Prompt("Should this script dynamically create/override <Shorthand> variables? (y/n)", "n")
                                        .StartsWith("y", StringComparison.OrdinalIgnoreCase)
                }
            };

            Directory.CreateDirectory(moduleId);
            string dummyScriptPath = Path.Combine(moduleId, scriptFile);
            if (scriptLang == "python")
            {
                File.WriteAllText(dummyScriptPath, "#!/usr/bin/env python3\nimport sys\nprint('Hello from Quarkit Module!')\n# To pass variables back, print: MyVar=Value\n");
            }
            else
            {
                File.WriteAllText(dummyScriptPath, "echo 'Hello from Quarkit Module!'");
            }
        }

        if (typeChoice == "2" || typeChoice == "3") // C Runtime or Hybrid
        {
            Console.WriteLine("\n--- C Runtime Configuration ---");
            string mainCFile = $"src/{moduleId}.c";

            cSources = new List<string> { mainCFile };
            compilerFlags = new List<string> { $"-DQUARKIT_{moduleId.ToUpperInvariant().Replace("-", "_")}_MODULE" };

            Directory.CreateDirectory(Path.Combine(moduleId, "src"));
            string fullCPath = Path.Combine(moduleId, mainCFile);
            File.WriteAllText(fullCPath, $"void quarkit_{moduleId.Replace("-", "_")}_init() {{\n    // Your module logic here \n}}\n");
        }

        var moduleManifest = new
        {
            Id = moduleId,
            Version = "1.0.0",
            Dependencies = new List<string>(),
            CSources = cSources,
            CompilerFlags = compilerFlags,
            PreBuildCommands = preBuildCommands
        };

        Directory.CreateDirectory(moduleId);
        string manifestPath = Path.Combine(moduleId, MODULE_FILENAME);

        JsonSerializerOptions options = new() { 
            WriteIndented = true, 
            DefaultIgnoreCondition = JsonIgnoreCondition.WhenWritingNull,
            Encoder = JavaScriptEncoder.UnsafeRelaxedJsonEscaping  
        };

        string jsonString = JsonSerializer.Serialize(moduleManifest, options);
        File.WriteAllText(manifestPath, jsonString);

        Console.ForegroundColor = ConsoleColor.Green;
        Console.WriteLine($"\n[✓] Successfully initialized module folder structure and manifest!");
        Console.WriteLine($"    Path: {Path.GetFullPath(moduleId)}/");
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
}