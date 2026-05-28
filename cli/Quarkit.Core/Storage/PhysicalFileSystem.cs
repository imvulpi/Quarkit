namespace Quarkit.Core.Storage;

/// <summary>
/// A default implementation of <see cref="IFileSystem"/> that uses <see cref="System.IO"/>.
/// </summary>
public class PhysicalFileSystem : IFileSystem
{
    public async Task<byte[]> ReadAllBytesAsync(string path)
    {
        return await File.ReadAllBytesAsync(path);
    }

    public async Task WriteAllBytesAsync(string path, byte[] data)
    {
        await File.WriteAllBytesAsync(path, data);
    }

    public string ReadAllText(string path)
    {
        return File.ReadAllText(path);
    }

    public void WriteAllText(string path, string data)
    {
        File.WriteAllText(path, data);
    }

    public bool FileExists(string path)
    {
        return File.Exists(path);
    }

    public bool DirectoryExists(string path)
    {
        return Directory.Exists(path);
    }

    public void DeleteFile(string path)
    {
        File.Delete(path);
    }

    public void DeleteDirectory(string path, bool recursive)
    {
        Directory.Delete(path, recursive);
    }

    public void CreateDirectory(string path)
    {
        Directory.CreateDirectory(path);
    }

    public IEnumerable<string> GetDirectories(string path)
    {
        return Directory.EnumerateDirectories(path);
    }

    public IEnumerable<string> GetFiles(string path)
    {
        return Directory.EnumerateFiles(path);
    }

    public Stream Open(string path, FileMode mode)
    {
        return File.Open(path, mode);
    }
}