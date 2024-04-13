void SpeechSynthesis::fireEvent(const AtomicString& type, SpeechSynthesisUtterance* utterance, unsigned long charIndex, const String& name)
{
    if (!executionContext()->activeDOMObjectsAreStopped())
        utterance->dispatchEvent(SpeechSynthesisEvent::create(type, charIndex, (currentTime() - utterance->startTime()), name));
}
