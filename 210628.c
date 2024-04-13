void AudioContext::derefFinishedSourceNodes()
{
    ASSERT(isGraphOwner());
    ASSERT(isAudioThread());
    for (unsigned i = 0; i < m_finishedNodes.size(); i++)
        derefNode(m_finishedNodes[i]);

    m_finishedNodes.clear();
}
