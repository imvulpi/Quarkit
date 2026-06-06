using Quarkit.Models.Core.Target;

namespace Quarkit.Core
{
    public static class Paths
    {
        public static string INSTALLERS_ROOT_DIR = "installer";
        public static string MODULES_ROOT_DIR = "modules";

        public static string GetModuleDir(string moduleName)
        {
            return Path.Join(MODULES_ROOT_DIR, moduleName);
        }

        public static string GetInstallerDir(OSSystem system)
        {
            return Path.Join(INSTALLERS_ROOT_DIR, system.AsString());
        }

        public static string GetInstallerIncludes(OSSystem system)
        {
            return Path.Join(INSTALLERS_ROOT_DIR, system.AsString(), "include");
        }

        public static string GetSharedInstallerIncludes()
        {
            return Path.Join(INSTALLERS_ROOT_DIR, "shared", "include");
        }
    }
}
