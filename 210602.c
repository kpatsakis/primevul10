size_t AudioContext::cachedSampleFrame() const
{
    ASSERT(isMainThread());

    return m_cachedSampleFrame;
}
