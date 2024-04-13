AudioContext::AudioNodeDisposer::~AudioNodeDisposer()
{
    ASSERT(isMainThread());
    AudioContext::AutoLocker locker(m_node.context());
    m_node.dispose();
}
