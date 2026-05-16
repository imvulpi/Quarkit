namespace Quarkit.Models.Core
{
    /// <summary>
    /// Defines the lifecycle stability levels of a version.
    /// </summary>
    /// <remarks>
    /// Note: These stages are intended as organizational suggestions. 
    /// Interpretation of stability and feature-completeness varies by organization; 
    /// for example, a "Beta" in one project may be more stable than a "Stable" release in another.
    /// </remarks>
    public enum QkVersionStage : byte
    {
        /// <summary>
        /// Initial development phase. Usually internal-only and potentially unstable.
        /// </summary>
        Development = 0,

        /// <summary> 
        /// Major bugs are likely; contains partial features planned for the eventual stable release. 
        /// </summary>
        Alpha = 1,

        /// <summary> 
        /// Relatively stable; usually feature-complete enough for public testing or "Open Beta" programs. 
        /// </summary>
        Beta = 2,

        /// <summary>
        /// Release Candidate. The "final" build intended for production, pending a final round of QA.
        /// </summary>
        Rc = 3,

        /// <summary>
        /// Production-ready. The official supported release for all users. 
        /// </summary>
        Stable = 4,
    }
}
