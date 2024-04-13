DynamicsCompressorNode* AudioContext::createDynamicsCompressor()
{
    ASSERT(isMainThread());
    return DynamicsCompressorNode::create(this, m_destinationNode->sampleRate());
}
