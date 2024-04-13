ScriptProcessorNode* AudioContext::createScriptProcessor(size_t bufferSize, size_t numberOfInputChannels, size_t numberOfOutputChannels, ExceptionState& exceptionState)
{
    ASSERT(isMainThread());
    ScriptProcessorNode* node = ScriptProcessorNode::create(this, m_destinationNode->sampleRate(), bufferSize, numberOfInputChannels, numberOfOutputChannels);

    if (!node) {
        if (!numberOfInputChannels && !numberOfOutputChannels) {
            exceptionState.throwDOMException(
                IndexSizeError,
                "number of input channels and output channels cannot both be zero.");
        } else if (numberOfInputChannels > AudioContext::maxNumberOfChannels()) {
            exceptionState.throwDOMException(
                IndexSizeError,
                "number of input channels (" + String::number(numberOfInputChannels)
                + ") exceeds maximum ("
                + String::number(AudioContext::maxNumberOfChannels()) + ").");
        } else if (numberOfOutputChannels > AudioContext::maxNumberOfChannels()) {
            exceptionState.throwDOMException(
                IndexSizeError,
                "number of output channels (" + String::number(numberOfInputChannels)
                + ") exceeds maximum ("
                + String::number(AudioContext::maxNumberOfChannels()) + ").");
        } else {
            exceptionState.throwDOMException(
                IndexSizeError,
                "buffer size (" + String::number(bufferSize)
                + ") must be a power of two between 256 and 16384.");
        }
        return 0;
    }

    refNode(node); // context keeps reference until we stop making javascript rendering callbacks
    return node;
}
