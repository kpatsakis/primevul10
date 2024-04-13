void SpeechSynthesis::pause()
{
    if (!m_isPaused)
        m_platformSpeechSynthesizer->pause();
}
