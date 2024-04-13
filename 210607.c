AudioBuffer* AudioContext::createBuffer(unsigned numberOfChannels, size_t numberOfFrames, float sampleRate, ExceptionState& exceptionState)
{
    return AudioBuffer::create(numberOfChannels, numberOfFrames, sampleRate, exceptionState);
}
