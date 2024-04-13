void SpeechSynthesis::cancel()
{
    m_utteranceQueue.clear();
    m_platformSpeechSynthesizer->cancel();
}
