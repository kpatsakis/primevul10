void AudioContext::disposeOutputs(AudioNode& node)
{
    ASSERT(isGraphOwner());
    ASSERT(isMainThread());
    for (unsigned i = 0; i < node.numberOfOutputs(); ++i)
        node.output(i)->dispose();
}
