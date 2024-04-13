void AudioContext::derefNode(AudioNode* node)
{
    ASSERT(isGraphOwner());

    for (unsigned i = 0; i < m_referencedNodes.size(); ++i) {
        if (node == m_referencedNodes.at(i).get()) {
            node->breakConnection();
            m_referencedNodes.remove(i);
            break;
        }
    }
}
