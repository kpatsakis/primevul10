void AudioContext::handlePostRenderTasks()
{
    ASSERT(isAudioThread());

    if (tryLock()) {
        updateChangedChannelCountMode();

        handleDeferredAudioNodeTasks();

        derefFinishedSourceNodes();

        handleDirtyAudioSummingJunctions();
        handleDirtyAudioNodeOutputs();

        updateAutomaticPullNodes();
        resolvePromisesForSuspend();

        unlock();
    }
}
