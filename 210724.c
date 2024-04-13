ScriptProcessorNode* AudioContext::createScriptProcessor(size_t bufferSize, size_t numberOfInputChannels, ExceptionState& exceptionState)
{
    return createScriptProcessor(bufferSize, numberOfInputChannels, 2, exceptionState);
}
