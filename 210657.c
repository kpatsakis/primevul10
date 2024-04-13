void AudioContext::resolvePromisesForResume()
{
    ASSERT(isAudioThread());
    ASSERT(isGraphOwner());

    if (!m_isResolvingResumePromises && m_resumeResolvers.size() > 0) {
        m_isResolvingResumePromises = true;
        callOnMainThread(bind(&AudioContext::resolvePromisesForResumeOnMainThread, this));
    }
}
