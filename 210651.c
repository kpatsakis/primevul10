void AudioContext::registerLiveAudioSummingJunction(AudioSummingJunction& junction)
{
    ASSERT(isMainThread());
    m_liveAudioSummingJunctions.add(&junction, adoptPtr(new AudioSummingJunctionDisposer(junction)));
}
