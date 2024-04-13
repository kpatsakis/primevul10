ChannelSplitterNode* AudioContext::createChannelSplitter(ExceptionState& exceptionState)
{
    const unsigned ChannelSplitterDefaultNumberOfOutputs = 6;
    return createChannelSplitter(ChannelSplitterDefaultNumberOfOutputs, exceptionState);
}
