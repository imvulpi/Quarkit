using Quarkit.Core.Storage;
using System.Text;

namespace Quarkit.Tests.Mocks;

public class FileSystemNode
{
    public string Name { get; init; } = string.Empty;
    public bool IsDirectory { get; init; }
    public string? TextContent { get; set; }

    public Dictionary<string, FileSystemNode> Children { get; } = new(StringComparer.OrdinalIgnoreCase);

    public FileSystemNode(string name, bool isDirectory)
    {
        Name = name;
        IsDirectory = isDirectory;
    }
}

public class MockFileSystem : IFileSystem
{
    /// <summary>
    /// Normalizes \\ and \ to / so the path is correct with <see cref="MockFileSystem"/> paths that use /
    /// </summary>
    /// <param name="path">Some not normalized path</param>
    /// <returns>A normalized <see cref="MockFileSystem"/> path</returns>
    public static string Normalize(string path)
    {
        string newPath = path.Replace("\\\\", "/").TrimEnd('/');
        newPath = newPath.Replace("\\", "/").TrimEnd('/');
        return newPath;
    }

    private readonly FileSystemNode _root = new("Root", isDirectory: true);

    // Helper to tokenize paths cleanly regardless of slash orientation

    /// <summary>
    /// Tokenizes a path into a clean string array.
    /// </summary>
    /// <param name="path">Regular system path with \\, \ or / as separators</param>
    /// <returns>String array with path segments.</returns>
    private string[] SplitPath(string path)
    {
        return path.Split(new[] { "/", "\\", "\\\\" }, StringSplitOptions.RemoveEmptyEntries);
    }

    /// <summary>
    /// Traverses down the tree. Optionally creates directories along the way.
    /// </summary>
    /// <param name="path">Path to traverse to</param>
    /// <param name="createMissingDirs">Whether to create missing directories.</param>
    /// <returns>A file system node is one is present</returns>
    private FileSystemNode? NavigateTo(string path, bool createMissingDirs = false)
    {
        var segments = SplitPath(path);
        var current = _root;

        foreach (var segment in segments)
        {
            if (!current.Children.TryGetValue(segment, out var nextNode))
            {
                if (createMissingDirs)
                {
                    nextNode = new FileSystemNode(segment, isDirectory: true);
                    current.Children[segment] = nextNode;
                }
                else
                {
                    return null; // Path segment doesn't exist
                }
            }
            current = nextNode;
        }

        return current;
    }

    public void CreateDirectory(string path)
    {
        NavigateTo(path, createMissingDirs: true);
    }

    public bool DirectoryExists(string path)
    {
        var node = NavigateTo(path);
        return node != null && node.IsDirectory;
    }

    public bool FileExists(string path)
    {
        var node = NavigateTo(path);
        return node != null && !node.IsDirectory;
    }

    public void WriteAllText(string path, string data)
    {
        var segments = SplitPath(path);
        if (segments.Length == 0) return;

        // Resolves or creates the parent directories
        string? parentPath = Path.GetDirectoryName(path);
        var parentNode = string.IsNullOrEmpty(parentPath) ? _root : NavigateTo(parentPath, createMissingDirs: true);

        if (parentNode == null || !parentNode.IsDirectory)
            throw new IOException($"Cannot write file to an invalid directory path: {path}");

        string fileName = segments[^1];
        if (!parentNode.Children.TryGetValue(fileName, out var fileNode) || parentNode.Children[fileName].IsDirectory)
        {
            fileNode = new FileSystemNode(fileName, isDirectory: false);
            parentNode.Children[fileName] = fileNode;
        }

        fileNode.TextContent = data;
    }

    public string ReadAllText(string path)
    {
        var node = NavigateTo(path);
        if (node == null || node.IsDirectory) throw new FileNotFoundException($"Mock file not found: {path}");
        return node.TextContent ?? string.Empty;
    }

    public IEnumerable<string> GetDirectories(string path)
    {
        var node = NavigateTo(path);
        if (node == null || !node.IsDirectory) return Enumerable.Empty<string>();

        return node.Children.Values
            .Where(c => c.IsDirectory)
            .Select(c => Path.Combine(path, c.Name));
    }

    public IEnumerable<string> GetFiles(string path)
    {
        var node = NavigateTo(path);
        if (node == null || !node.IsDirectory) return Enumerable.Empty<string>();

        return node.Children.Values
            .Where(c => !c.IsDirectory)
            .Select(c => Path.Combine(path, c.Name));
    }

    // API Compatibility Wrappers
    public Task<byte[]> ReadAllBytesAsync(string path) => Task.FromResult(Encoding.UTF8.GetBytes(ReadAllText(path)));
    public Task WriteAllBytesAsync(string path, byte[] data) { WriteAllText(path, Encoding.UTF8.GetString(data)); return Task.CompletedTask; }
    public void DeleteFile(string path) => NavigateTo(Path.GetDirectoryName(path) ?? "")?.Children.Remove(Path.GetFileName(path));
    public void DeleteDirectory(string path, bool recursive) => NavigateTo(Path.GetDirectoryName(path) ?? "")?.Children.Remove(Path.GetFileName(path));
    public Stream Open(string path, FileMode mode)
    {
        var memStream = new MemoryStream();
        if (mode == FileMode.Open && FileExists(path))
        {
            var bytes = Encoding.UTF8.GetBytes(ReadAllText(path));
            memStream.Write(bytes, 0, bytes.Length);
            memStream.Position = 0;
        }
        return memStream;
    }

    public byte[] ReadAllBytes(string path)
    {
        return Encoding.UTF8.GetBytes(ReadAllText(path));
    }

    public IEnumerable<string> ReadLines(string path)
    {
        var line = ReadAllText(path);
        return line.Split(["\n", "\r\n"], StringSplitOptions.None).AsEnumerable();
    }

    public void AppendAllBytes(string path, byte[] data)
    {
        byte[] existingBytes = ReadAllBytes(path);
        byte[] newBytes = new byte[existingBytes.Length + data.Length];
        Array.Copy(existingBytes, newBytes, 0);
        Array.Copy(data, 0, newBytes, existingBytes.Length, data.Length);
        WriteAllText(path, Encoding.UTF8.GetString(newBytes));
    }
}