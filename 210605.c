AnalyserNode* AudioContext::createAnalyser()
{
    ASSERT(isMainThread());
    return AnalyserNode::create(this, m_destinationNode->sampleRate());
}
