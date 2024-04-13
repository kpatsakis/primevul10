bool SpeechSynthesis::pending() const
{
    return m_utteranceQueue.size() > 1;
}
