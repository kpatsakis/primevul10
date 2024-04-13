AudioContext::AudioSummingJunctionDisposer::~AudioSummingJunctionDisposer()
{
    ASSERT(isMainThread());
    m_junction.dispose();
}
