void AudioContext::setContextState(AudioContextState newState)
{
    ASSERT(isMainThread());

    switch (newState) {
    case Suspended:
        ASSERT(m_contextState == Running);
        break;
    case Running:
        ASSERT(m_contextState == Suspended);
        break;
    case Closed:
        ASSERT(m_contextState != Closed);
        break;
    }

    if (newState == m_contextState) {
        return;
    }

    m_contextState = newState;

    if (executionContext())
        executionContext()->postTask(createSameThreadTask(&AudioContext::notifyStateChange, this));
}
