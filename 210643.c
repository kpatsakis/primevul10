bool AudioContext::isAudioThread() const
{
    return currentThread() == m_audioThread;
}
