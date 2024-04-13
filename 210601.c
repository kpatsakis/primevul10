void AudioContext::addDeferredBreakConnection(AudioNode& node)
{
    ASSERT(isAudioThread());
    m_deferredBreakConnectionList.append(&node);
}
