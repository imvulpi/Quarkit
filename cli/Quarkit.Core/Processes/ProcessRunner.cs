using System.Diagnostics;
using System.Text;

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

        using var process = new Process { StartInfo = startInfo };
        var outputBuilder = new StringBuilder();
        var errorBuilder = new StringBuilder();
        process.OutputDataReceived += (sender, e) => { if (e.Data != null) outputBuilder.AppendLine(e.Data); };
        process.ErrorDataReceived += (sender, e) => { if (e.Data != null) errorBuilder.AppendLine(e.Data); };

        if (process.Start())
        {
            process.BeginOutputReadLine();
            process.BeginErrorReadLine();
            process.WaitForExit();
            return (process.ExitCode, outputBuilder.ToString(), errorBuilder.ToString());
        }

        return (0, string.Empty, string.Empty);
    }
}