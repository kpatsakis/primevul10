void SpeechSynthesis::startSpeakingImmediately()
{
    SpeechSynthesisUtterance* utterance = currentSpeechUtterance();
    ASSERT(utterance);

    utterance->setStartTime(monotonicallyIncreasingTime());
    m_isPaused = false;
    m_platformSpeechSynthesizer->speak(utterance->platformUtterance());
}
