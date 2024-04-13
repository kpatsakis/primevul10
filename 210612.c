ConvolverNode* AudioContext::createConvolver()
{
    ASSERT(isMainThread());
    return ConvolverNode::create(this, m_destinationNode->sampleRate());
}
