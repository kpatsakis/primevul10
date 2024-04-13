void AudioContext::initialize()
{
    if (isInitialized())
        return;

    FFTFrame::initialize();
    m_listener = AudioListener::create();

    if (m_destinationNode.get()) {
        m_destinationNode->initialize();

        if (!isOfflineContext()) {
            startRendering();
            ++s_hardwareContextCount;
        }

        m_isInitialized = true;
    }
}
