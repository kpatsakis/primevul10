WaveShaperNode* AudioContext::createWaveShaper()
{
    ASSERT(isMainThread());
    return WaveShaperNode::create(this);
}
