void AudioContext::uninitialize()
{
    ASSERT(isMainThread());

    if (!isInitialized())
        return;

    m_isInitialized = false;

    m_destinationNode->uninitialize();

    if (!isOfflineContext()) {
        ASSERT(s_hardwareContextCount);
        --s_hardwareContextCount;
    }

    derefUnfinishedSourceNodes();

    rejectPendingResolvers();

    ASSERT(m_listener);
    m_listener->waitForHRTFDatabaseLoaderThreadCompletion();

    clear();
}
