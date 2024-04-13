void AudioContext::decodeAudioData(DOMArrayBuffer* audioData, AudioBufferCallback* successCallback, AudioBufferCallback* errorCallback, ExceptionState& exceptionState)
{
    if (!audioData) {
        exceptionState.throwDOMException(
            SyntaxError,
            "invalid ArrayBuffer for audioData.");
        return;
    }
    m_audioDecoder.decodeAsync(audioData, sampleRate(), successCallback, errorCallback);
}
