MediaElementAudioSourceNode* AudioContext::createMediaElementSource(HTMLMediaElement* mediaElement, ExceptionState& exceptionState)
{
    ASSERT(isMainThread());
    if (!mediaElement) {
        exceptionState.throwDOMException(
            InvalidStateError,
            "invalid HTMLMedialElement.");
        return 0;
    }

    if (mediaElement->audioSourceNode()) {
        exceptionState.throwDOMException(
            InvalidStateError,
            "HTMLMediaElement already connected previously to a different MediaElementSourceNode.");
        return 0;
    }

    MediaElementAudioSourceNode* node = MediaElementAudioSourceNode::create(this, mediaElement);

    mediaElement->setAudioSourceNode(node);

    refNode(node); // context keeps reference until node is disconnected
    return node;
}
