void AudioContext::stop()
{
    if (m_isStopScheduled)
        return;
    m_isStopScheduled = true;

    callOnMainThread(bind(&AudioContext::uninitialize, this));
}
