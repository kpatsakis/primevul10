MediaStreamAudioSourceNode* AudioContext::createMediaStreamSource(MediaStream* mediaStream, ExceptionState& exceptionState)
{
    ASSERT(isMainThread());
    if (!mediaStream) {
        exceptionState.throwDOMException(
            InvalidStateError,
            "invalid MediaStream source");
        return 0;
    }

    MediaStreamTrackVector audioTracks = mediaStream->getAudioTracks();
    if (audioTracks.isEmpty()) {
        exceptionState.throwDOMException(
            InvalidStateError,
            "MediaStream has no audio track");
        return 0;
    }

    MediaStreamTrack* audioTrack = audioTracks[0];
    OwnPtr<AudioSourceProvider> provider = audioTrack->createWebAudioSource();
    MediaStreamAudioSourceNode* node = MediaStreamAudioSourceNode::create(this, mediaStream, audioTrack, provider.release());

    node->setFormat(2, sampleRate());

    refNode(node); // context keeps reference until node is disconnected
    return node;
}
