void SpeechSynthesis::voicesDidChange()
{
    m_voiceList.clear();
    if (!executionContext()->activeDOMObjectsAreStopped())
        dispatchEvent(Event::create(EventTypeNames::voiceschanged));
}
