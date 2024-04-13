void AudioContext::stopRendering()
{
    ASSERT(isMainThread());
    ASSERT(m_destinationNode);
    ASSERT(!isOfflineContext());

    if (m_contextState == Running) {
        destination()->stopRendering();
        setContextState(Suspended);
    }
}
