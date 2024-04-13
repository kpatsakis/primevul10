DelayNode* AudioContext::createDelay(ExceptionState& exceptionState)
{
    const double defaultMaxDelayTime = 1;
    return createDelay(defaultMaxDelayTime, exceptionState);
}
