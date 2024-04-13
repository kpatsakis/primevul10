ScriptProcessorNode* AudioContext::createScriptProcessor(size_t bufferSize, ExceptionState& exceptionState)
{
    return createScriptProcessor(bufferSize, 2, 2, exceptionState);
}
