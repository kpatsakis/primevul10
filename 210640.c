bool AudioContext::hasPendingActivity() const
{
    return !m_isCleared;
}
