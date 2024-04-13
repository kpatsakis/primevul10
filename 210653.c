void AudioContext::rejectPendingResolvers()
{
    ASSERT(isMainThread());


    for (auto& resolver : m_suspendResolvers) {
        resolver->reject(DOMException::create(InvalidStateError, "Audio context is going away"));
    }
    m_suspendResolvers.clear();

    for (auto& resolver : m_resumeResolvers) {
        resolver->reject(DOMException::create(InvalidStateError, "Audio context is going away"));
    }
    m_resumeResolvers.clear();
    m_isResolvingResumePromises = false;
}
