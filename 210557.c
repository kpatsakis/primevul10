SpeechSynthesisUtterance* SpeechSynthesis::currentSpeechUtterance() const
{
    if (!m_utteranceQueue.isEmpty())
        return m_utteranceQueue.first().get();
    return 0;
}
