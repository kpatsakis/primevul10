PannerNode* AudioContext::createPanner()
{
    ASSERT(isMainThread());
    return PannerNode::create(this, m_destinationNode->sampleRate());
}
