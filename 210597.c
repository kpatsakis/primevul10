AudioContext::AudioContext(Document* document)
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
    , m_isOfflineContext(false)
    , m_contextState(Suspended)
    , m_cachedSampleFrame(0)
{
    m_didInitializeContextGraphMutex = true;
    m_destinationNode = DefaultAudioDestinationNode::create(this);

    initialize();
#if DEBUG_AUDIONODE_REFERENCES
    fprintf(stderr, "%p: AudioContext::AudioContext() #%u\n", this, AudioContext::s_hardwareContextCount);
#endif
}
