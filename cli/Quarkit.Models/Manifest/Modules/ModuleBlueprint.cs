namespace Quarkit.Models.Manifest.Modules
{
    public class ModuleBlueprint : IMergeable<ModuleBlueprint>
    {
        public bool? HasInitHook { get; set; }
        public bool? HasDeInitHook { get; set; }
        public ModuleDelta? CSources { get; set; }
        public ModuleDelta? CompilerFlags { get; set; }
        public ModuleDelta? Dependencies { get; set; }
        public List<ModuleCommand>? PreBuildCommands { get; set; }

        public void MergeFrom(ModuleBlueprint source)
        {
            // This overrides all the values. Doesn't append. (Possible TODO).
            if(source.HasInitHook != null) HasInitHook = source.HasInitHook.Value;
            if(source.HasDeInitHook != null) HasDeInitHook = source.HasDeInitHook.Value;
            if(source.PreBuildCommands != null) PreBuildCommands = source.PreBuildCommands;
            Dependencies = MergeDelta(Dependencies, source.Dependencies);
            CSources = MergeDelta(CSources, source.CSources);
            CompilerFlags = MergeDelta(CompilerFlags, source.CompilerFlags);
        }

        private static ModuleDelta? MergeDelta(ModuleDelta? current, ModuleDelta? incoming)
        {
            if (incoming == null) return current;
            if (incoming.IsFullOverwrite)
            {
                return incoming;
            }

            var baseList = (current != null && current.IsFullOverwrite)
                ? [.. current.Values ?? []]
                : new HashSet<string>();

            if (incoming.Remove != null)
            {
                foreach (var item in incoming.Remove) baseList.Remove(item);
            }

            if (incoming.Add != null)
            {
                foreach (var item in incoming.Add) baseList.Add(item);
            }

            return new ModuleDelta { Values = [.. baseList] };
        }
    }
}
