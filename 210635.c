void AudioContext::handleDirtyAudioNodeOutputs()
{
    ASSERT(isGraphOwner());

    for (HashSet<AudioNodeOutput*>::iterator i = m_dirtyAudioNodeOutputs.begin(); i != m_dirtyAudioNodeOutputs.end(); ++i)
        (*i)->updateRenderingState();

    m_dirtyAudioNodeOutputs.clear();
}
