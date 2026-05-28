using System.Diagnostics;

namespace Quarkit.Core.Processes;

public class ProcessRunner : IProcessRunner
{
    public (int ExitCode, string Output, string Error) Execute(string filename, string arguments)
    {
        var startInfo = new ProcessStartInfo
        {
            FileName = filename,
            Arguments = arguments,
            RedirectStandardOutput = true,
            RedirectStandardError = true,
            UseShellExecute = false,
            CreateNoWindow = true
        };

        using var process = Process.Start(startInfo);
        if (process != null)
        {
            string output = process.StandardOutput.ReadToEnd();
            string error = process.StandardError.ReadToEnd();
            process.WaitForExit();
            return (process.ExitCode, output, error);
        }

        return (0, string.Empty, string.Empty);
    }
}