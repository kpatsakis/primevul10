BiquadFilterNode* AudioContext::createBiquadFilter()
{
    ASSERT(isMainThread());
    return BiquadFilterNode::create(this, m_destinationNode->sampleRate());
}
