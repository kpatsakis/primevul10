void SpeechSynthesis::resume()
{
    if (!currentSpeechUtterance())
        return;
    m_platformSpeechSynthesizer->resume();
}
