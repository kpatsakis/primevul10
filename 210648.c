void AudioContext::notifyStateChange()
{
    dispatchEvent(Event::create(EventTypeNames::statechange));
}
