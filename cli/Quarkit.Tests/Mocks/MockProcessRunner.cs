using Quarkit.Core.Processes;

namespace Quarkit.Tests.Mocks;

public class MockProcessRunner : IProcessRunner
{
    public class ExecutedCommand
    {
        public string Filename { get; set; } = string.Empty;
        public string Arguments { get; set; } = string.Empty;
    }

    public List<ExecutedCommand> History { get; } = new();
    public Dictionary<string, (int ExitCode, string Output, string Error)> PlannedOutputs { get; set; } = new(StringComparer.OrdinalIgnoreCase);

    public (int ExitCode, string Output, string Error) Execute(string filename, string arguments)
    {
        History.Add(new ExecutedCommand { Filename = filename, Arguments = arguments });

        string setupKey = $"{filename} {arguments}".Trim();
        if (PlannedOutputs.TryGetValue(setupKey, out var output)) return output;
        if (PlannedOutputs.TryGetValue(filename, out var basicOutput)) return basicOutput;

        return (0, "Mock execution success", string.Empty);
    }
}