namespace Quarkit.Models.Manifest;
public interface IBlueprintOverride<T>
{
    public T Value { get; set; }
    public TargetKey? TargetKey { get; set; }
    public List<string>? Expressions { get; set; }
}

public interface IMergeable<T>
{
    public void MergeFrom(T source);
}
