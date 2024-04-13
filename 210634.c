void AudioContext::handleDeferredAudioNodeTasks()
{
    ASSERT(isAudioThread() && isGraphOwner());

    for (unsigned i = 0; i < m_deferredBreakConnectionList.size(); ++i)
        m_deferredBreakConnectionList[i]->breakConnectionWithLock();
    m_deferredBreakConnectionList.clear();
}
