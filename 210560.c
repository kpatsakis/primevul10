void SpeechSynthesis::didStartSpeaking(PassRefPtr<PlatformSpeechSynthesisUtterance> utterance)
{
    if (utterance->client())
        fireEvent(EventTypeNames::start, static_cast<SpeechSynthesisUtterance*>(utterance->client()), 0, String());
}
