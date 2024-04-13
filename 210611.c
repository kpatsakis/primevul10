ChannelSplitterNode* AudioContext::createChannelSplitter(size_t numberOfOutputs, ExceptionState& exceptionState)
{
    ASSERT(isMainThread());

    ChannelSplitterNode* node = ChannelSplitterNode::create(this, m_destinationNode->sampleRate(), numberOfOutputs);

    if (!node) {
        exceptionState.throwDOMException(
            IndexSizeError,
            "number of outputs (" + String::number(numberOfOutputs)
            + ") must be between 1 and "
            + String::number(AudioContext::maxNumberOfChannels()) + ".");
        return 0;
    }

    return node;
}
