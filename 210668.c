void AudioContext::trace(Visitor* visitor)
{
    visitor->trace(m_offlineResolver);
    visitor->trace(m_renderTarget);
    visitor->trace(m_destinationNode);
    visitor->trace(m_listener);
    if (m_didInitializeContextGraphMutex) {
        AutoLocker lock(this);
        visitor->trace(m_referencedNodes);
    } else {
        visitor->trace(m_referencedNodes);
    }
    visitor->trace(m_resumeResolvers);
    visitor->trace(m_suspendResolvers);
    visitor->trace(m_liveNodes);
    visitor->trace(m_liveAudioSummingJunctions);
    RefCountedGarbageCollectedEventTargetWithInlineData<AudioContext>::trace(visitor);
    ActiveDOMObject::trace(visitor);
}
