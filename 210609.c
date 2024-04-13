ChannelMergerNode* AudioContext::createChannelMerger(size_t numberOfInputs, ExceptionState& exceptionState)
{
    ASSERT(isMainThread());

    ChannelMergerNode* node = ChannelMergerNode::create(this, m_destinationNode->sampleRate(), numberOfInputs);

    if (!node) {
        exceptionState.throwDOMException(
            IndexSizeError,
            "number of inputs (" + String::number(numberOfInputs)
            + ") must be between 1 and "
            + String::number(AudioContext::maxNumberOfChannels()) + ".");
        return 0;
    }

    return node;
}
