ExecutionContext* AudioContext::executionContext() const
{
    return m_isStopScheduled ? 0 : ActiveDOMObject::executionContext();
}
