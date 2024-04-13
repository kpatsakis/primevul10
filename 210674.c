AudioContext::~AudioContext()
{
#if DEBUG_AUDIONODE_REFERENCES
    fprintf(stderr, "%p: AudioContext::~AudioContext()\n", this);
#endif
    ASSERT(!m_isInitialized);
    ASSERT(!m_referencedNodes.size());
    ASSERT(!m_finishedNodes.size());
    ASSERT(!m_automaticPullNodes.size());
    if (m_automaticPullNodesNeedUpdating)
        m_renderingAutomaticPullNodes.resize(m_automaticPullNodes.size());
    ASSERT(!m_renderingAutomaticPullNodes.size());
    ASSERT(!m_suspendResolvers.size());
    ASSERT(!m_resumeResolvers.size());
}
