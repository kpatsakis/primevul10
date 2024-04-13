void AudioContext::handleDirtyAudioSummingJunctions()
{
    ASSERT(isGraphOwner());

    for (HashSet<AudioSummingJunction*>::iterator i = m_dirtySummingJunctions.begin(); i != m_dirtySummingJunctions.end(); ++i)
        (*i)->updateRenderingState();

    m_dirtySummingJunctions.clear();
}
