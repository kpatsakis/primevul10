void AudioContext::startRendering()
{
    ASSERT(isMainThread());
    ASSERT(m_destinationNode);

    if (m_contextState == Suspended) {
        destination()->startRendering();
        setContextState(Running);
    }
}
