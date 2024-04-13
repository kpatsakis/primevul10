StereoPannerNode* AudioContext::createStereoPanner()
{
    ASSERT(isMainThread());
    return StereoPannerNode::create(this, m_destinationNode->sampleRate());
}
