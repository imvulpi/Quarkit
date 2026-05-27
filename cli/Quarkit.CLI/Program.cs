using Quarkit.CLI;
using Quarkit.Models.Manifest;
using System.Text.Json;

internal class Program
{
    public class QuarkitArgumentSettings
    {
        public bool ShouldContinue { get; set; } = true;
    }

    private static void Main(string[] args)
    {
        QuarkitArgumentSettings settings = HandleArgs(args);
        if (settings.ShouldContinue) return;
    }

    static QuarkitArgumentSettings HandleArgs(string[] args)
    {
        if (args.Length == 0) return new();

        QuarkitArgumentSettings settings = new();
        List<string> arguments = [];

        for (int i = 0; i < args.Length; i++)
        {
            string arg = args[i];
            foreach (string splitArg in arg.Split("="))
            {
                if (splitArg.Trim() == "") continue;
                arguments.Add(splitArg);
            }
        }

        for (int i = 0; arguments.Count > i; i++)
        {
            string argument = arguments[i];
            argument = argument.Replace("--", "");
            switch (argument)
            {
                case "new":
                    ManifestWizard.Run();
                    break;

                case "help":
                    settings.ShouldContinue = false;
                    DisplayHelpScreen();
                    break;
                default:
                    break;
            }
        }

        return settings;
    }

    static void DisplayHelpScreen()
    {
        string message = "Welcome to Quarkit.\n" +
                         "Arguments:\n" +
                         "  --help         | You are here :)\n" +
                         "  --new          | Setup for a new installer manifest.";
        Console.WriteLine(message);
    }

    static bool EnsureHasValue(int currentIndex, int totalCount, string flagName)
    {
        if (currentIndex + 1 < totalCount) return true;

        Console.WriteLine($"Error: The argument '{flagName}' requires a value after it.");
        return false;
    }
}