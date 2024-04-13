AudioContext* AudioContext::create(Document& document, ExceptionState& exceptionState)
{
    ASSERT(isMainThread());
    if (s_hardwareContextCount >= MaxHardwareContexts) {
        exceptionState.throwDOMException(
            SyntaxError,
            "number of hardware contexts reached maximum (" + String::number(MaxHardwareContexts) + ").");
        return 0;
    }

    AudioContext* audioContext = new AudioContext(&document);
    audioContext->suspendIfNeeded();
    return audioContext;
}
