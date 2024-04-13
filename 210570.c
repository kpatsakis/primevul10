void SpeechSynthesis::speak(SpeechSynthesisUtterance* utterance, ExceptionState& exceptionState)
{
    if (!utterance) {
        exceptionState.throwTypeError("Invalid utterance argument");
        return;
    }

    m_utteranceQueue.append(utterance);

    if (m_utteranceQueue.size() == 1)
        startSpeakingImmediately();
}
