void AudioContext::removeMarkedSummingJunction(AudioSummingJunction* summingJunction)
{
    ASSERT(isMainThread());
    AutoLocker locker(this);
    m_dirtySummingJunctions.remove(summingJunction);
}
