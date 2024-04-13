GainNode* AudioContext::createGain()
{
    ASSERT(isMainThread());
    return GainNode::create(this, m_destinationNode->sampleRate());
}
