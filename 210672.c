void AudioContext::updateAutomaticPullNodes()
{
    ASSERT(isGraphOwner());

    if (m_automaticPullNodesNeedUpdating) {
        m_renderingAutomaticPullNodes.resize(m_automaticPullNodes.size());

        unsigned j = 0;
        for (HashSet<AudioNode*>::iterator i = m_automaticPullNodes.begin(); i != m_automaticPullNodes.end(); ++i, ++j) {
            AudioNode* output = *i;
            m_renderingAutomaticPullNodes[j] = output;
        }

        m_automaticPullNodesNeedUpdating = false;
    }
}
