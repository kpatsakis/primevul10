void AudioContext::lock()
{
    ASSERT(isMainThread());
    m_contextGraphMutex.lock();
}
