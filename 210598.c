AudioContext::AudioContext(Document* document, unsigned numberOfChannels, size_t numberOfFrames, float sampleRate)
    : ActiveDOMObject(document)
    , m_isStopScheduled(false)
    , m_isCleared(false)
    , m_isInitialized(false)
    , m_destinationNode(nullptr)
    , m_isResolvingResumePromises(false)
    , m_automaticPullNodesNeedUpdating(false)
    , m_connectionCount(0)
    , m_didInitializeContextGraphMutex(false)
    , m_audioThread(0)
    , m_isOfflineContext(true)
    , m_contextState(Suspended)
    , m_cachedSampleFrame(0)
{
    m_didInitializeContextGraphMutex = true;
    m_renderTarget = AudioBuffer::create(numberOfChannels, numberOfFrames, sampleRate);
    if (m_renderTarget.get())
        m_destinationNode = OfflineAudioDestinationNode::create(this, m_renderTarget.get());

    initialize();
}
