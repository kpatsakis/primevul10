ScriptProcessorNode* AudioContext::createScriptProcessor(ExceptionState& exceptionState)
{
    return createScriptProcessor(0, 2, 2, exceptionState);
}
