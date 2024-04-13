void SpeechSynthesis::trace(Visitor* visitor)
{
    visitor->trace(m_voiceList);
    visitor->trace(m_utteranceQueue);
}
