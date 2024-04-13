void AudioContext::handlePreRenderTasks()
{
    ASSERT(isAudioThread());

    if (tryLock()) {
        updateChangedChannelCountMode();

        handleDirtyAudioSummingJunctions();
        handleDirtyAudioNodeOutputs();

        updateAutomaticPullNodes();
        resolvePromisesForResume();

        handleStoppableSourceNodes();

        m_cachedSampleFrame = currentSampleFrame();

        unlock();
    }
}
