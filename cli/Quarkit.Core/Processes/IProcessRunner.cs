namespace Quarkit.Core.Processes
{
    public interface IProcessRunner
    {
        (int ExitCode, string Output, string Error) Execute(string filename, string arguments);
    }
}