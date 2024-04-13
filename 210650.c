void AudioContext::refNode(AudioNode* node)
{
    ASSERT(isMainThread());
    AutoLocker locker(this);

    m_referencedNodes.append(node);
    node->makeConnection();
}
