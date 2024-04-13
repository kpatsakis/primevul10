void SpeechSynthesis::boundaryEventOccurred(PassRefPtr<PlatformSpeechSynthesisUtterance> utterance, SpeechBoundary boundary, unsigned charIndex)
{
    DEFINE_STATIC_LOCAL(const String, wordBoundaryString, ("word"));
    DEFINE_STATIC_LOCAL(const String, sentenceBoundaryString, ("sentence"));

    switch (boundary) {
    case SpeechWordBoundary:
        fireEvent(EventTypeNames::boundary, static_cast<SpeechSynthesisUtterance*>(utterance->client()), charIndex, wordBoundaryString);
        break;
    case SpeechSentenceBoundary:
        fireEvent(EventTypeNames::boundary, static_cast<SpeechSynthesisUtterance*>(utterance->client()), charIndex, sentenceBoundaryString);
        break;
    default:
        ASSERT_NOT_REACHED();
    }
}
