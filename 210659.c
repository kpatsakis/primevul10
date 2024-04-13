void AudioContext::resolvePromisesForSuspend()
{
    ASSERT(isAudioThread());
    ASSERT(isGraphOwner());

    if (m_suspendResolvers.size() > 0)
        callOnMainThread(bind(&AudioContext::resolvePromisesForSuspendOnMainThread, this));

}
