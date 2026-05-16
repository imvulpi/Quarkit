namespace Quarkit.Models.Core
{
    /// <summary>
    /// Defines the distribution channel (the "Pipe") used to deliver a version.
    /// </summary>
    /// <remarks>
    /// Channels follow a "Waterfall" logic. Users on a lower-indexed channel (e.g., Canary) 
    /// should receive updates from higher-indexed channels (e.g., Release) if the 
    /// version in the higher channel is newer/more stable than their current build.
    /// </remarks>
    public enum QkBuildChannel : ushort
    {
        /// <summary> 
        /// Internal-only. The rawest form of the software. 
        /// Typically maps to the <see cref="QkVersionStage.Development"/> stage.
        /// </summary>
        Development = 0,

        /// <summary> 
        /// Automated builds. Usually <see cref="QkVersionStage.Alpha"/>. 
        /// Often used by open-source contributors or internal automated testers.
        /// </summary>
        Nightly = 1,

        /// <summary> 
        /// First contact with external users. 
        /// Transitions from "Grown Alpha" to <see cref="QkVersionStage.Beta"/>.
        /// </summary>
        Canary = 2,

        /// <summary> 
        /// Feature-complete builds focused on bug hunting. 
        /// Transitions from <see cref="QkVersionStage.Beta"/> to <see cref="QkVersionStage.Rc"/>.
        /// </summary>
        Testing = 3,

        /// <summary> 
        /// The primary public channel. 
        /// Contains the latest <see cref="QkVersionStage.Stable"/> releases.
        /// </summary>
        Release = 4,

        /// <summary> 
        /// Long-Term Support. Dedicated streams (via Sub-IDs) for organizations 
        /// requiring high compatibility and only critical security updates.
        /// </summary>
        Lts = 5,

        /// <summary> 
        /// Retired versions or builds for deprecated hardware/OS environments.
        /// </summary>
        Legacy = 6,

        /// <summary> Reserved for future standardized Qk channels (7-31). </summary>
        Reserved = 7,

        /// <summary> Starting point for developer-defined custom channels (32-...). </summary>
        CustomStart = 32
    }
}
