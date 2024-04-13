void AudioContext::resolvePromisesForSuspendOnMainThread()
{
    ASSERT(isMainThread());
    AutoLocker locker(this);

    if (m_destinationNode)
        stopRendering();

    for (auto& resolver : m_suspendResolvers) {
        if (m_contextState == Closed) {
            resolver->reject(
                DOMException::create(InvalidStateError, "Cannot suspend a context that has been closed"));
        } else {
            resolver->resolve();
        }
    }

    m_suspendResolvers.clear();
}
