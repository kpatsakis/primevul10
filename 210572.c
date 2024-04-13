void SpeechSynthesis::speakingErrorOccurred(PassRefPtr<PlatformSpeechSynthesisUtterance> utterance)
{
    if (utterance->client())
        handleSpeakingCompleted(static_cast<SpeechSynthesisUtterance*>(utterance->client()), true);
}
