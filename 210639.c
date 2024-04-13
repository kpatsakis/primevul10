void AudioContext::handleStoppableSourceNodes()
{
    ASSERT(isGraphOwner());

    for (unsigned i = 0; i < m_referencedNodes.size(); ++i) {
        AudioNode* node = m_referencedNodes.at(i).get();

        if (node->nodeType() == AudioNode::NodeTypeAudioBufferSource) {
            AudioBufferSourceNode* sourceNode = static_cast<AudioBufferSourceNode*>(node);
            sourceNode->handleStoppableSourceNode();
        }
    }
}
