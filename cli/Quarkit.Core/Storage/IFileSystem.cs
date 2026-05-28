namespace Quarkit.Core.Storage;

/// <summary>
/// Abstraction over the filesystem to allow for better testability and potential different implementations.
/// </summary>
public interface IFileSystem
{
    /// <summary>
    /// Asynchronously reads the content of the file at the specified path into a byte array.
    /// </summary>
    Task<byte[]> ReadAllBytesAsync(string path);

    /// <summary>
    /// Asynchronously writes the specified byte array to a file at the specified path.
    /// </summary>
    Task WriteAllBytesAsync(string path, byte[] data);

    /// <summary>
    /// Asynchronously reads the content of the file at the specified path into a string.
    /// </summary>
    string ReadAllText(string path);

    /// <summary>
    /// Asynchronously writes the specified byte string to a file at the specified path.
    /// </summary>
    void WriteAllText(string path, string data);

    /// <summary>
    /// Checks if a file exists at the specified path.
    /// </summary>
    bool FileExists(string path);

    /// <summary>
    /// Checks if a directory exists at the specified path.
    /// </summary>
    bool DirectoryExists(string path);

    /// <summary>
    /// Deletes the specified file.
    /// </summary>
    void DeleteFile(string path);

    /// <summary>
    /// Deletes the specified directory and, if indicated, any subdirectories and files in the directory.
    /// </summary>
    void DeleteDirectory(string path, bool recursive);

    /// <summary>
    /// Creates all directories and subdirectories in the specified path unless they already exist.
    /// </summary>
    void CreateDirectory(string path);

    /// <summary>
    /// Returns the paths of subdirectories in the specified directory. Paths don't have a trailing directory separator character.
    /// </summary>
    IEnumerable<string> GetDirectories(string path);

    /// <summary>
    /// Returns the paths of files in the specified directory.
    /// </summary>
    IEnumerable<string> GetFiles(string path);

    /// <summary>
    /// Opens a <see cref="FileStream"/> on the specified path, with the specified mode.
    /// </summary>
    Stream Open(string path, FileMode mode);
}