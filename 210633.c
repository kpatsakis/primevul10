void AudioContext::fireCompletionEvent()
{
    ASSERT(isMainThread());
    if (!isMainThread())
        return;

    AudioBuffer* renderedBuffer = m_renderTarget.get();

    setContextState(Closed);

    ASSERT(renderedBuffer);
    if (!renderedBuffer)
        return;

    if (executionContext()) {
        dispatchEvent(OfflineAudioCompletionEvent::create(renderedBuffer));
        m_offlineResolver->resolve(renderedBuffer);
    }
}
