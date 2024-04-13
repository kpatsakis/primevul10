    ~CSessionManager() {
        // Make sure all sessions are destroyed before any of our maps
        // are destroyed
        m_mspSessions.Clear();
    }