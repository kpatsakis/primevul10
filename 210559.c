void SpeechSynthesis::didResumeSpeaking(PassRefPtr<PlatformSpeechSynthesisUtterance> utterance)
{
    m_isPaused = false;
    if (utterance->client())
        fireEvent(EventTypeNames::resume, static_cast<SpeechSynthesisUtterance*>(utterance->client()), 0, String());
}
