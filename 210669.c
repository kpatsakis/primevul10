bool AudioContext::tryLock()
{
    ASSERT(isAudioThread());
    if (!isAudioThread()) {
        lock();
        return true;
    }
    return m_contextGraphMutex.tryLock();
}
