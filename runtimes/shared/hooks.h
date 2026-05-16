/// @brief Hooks of the installer that modules can use to define custom logic.
enum Hooks{
    /// @brief A hook immediately after the installer runs.
    INIT,
    
    /// @brief A hook before extraction of the payload.
    PRE_EXTRACT,
    
    /// @brief A hook after the payload was extracted.
    POST_EXTRACT,
    
    /// @brief A hook before registering the payloads in the system.
    PRE_REGISTER,
    
    /// @brief A hook after the payload was registered in the system.
    POST_REGISTER,
    
    /// @brief A hook for when installer finished. 
    FINISH,
    
    /// @brief A hook for when a rollback is going to happen.
    ROLLBACK
};