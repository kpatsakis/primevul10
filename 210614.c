DelayNode* AudioContext::createDelay(double maxDelayTime, ExceptionState& exceptionState)
{
    ASSERT(isMainThread());
    DelayNode* node = DelayNode::create(this, m_destinationNode->sampleRate(), maxDelayTime, exceptionState);
    if (exceptionState.hadException())
        return 0;
    return node;
}
