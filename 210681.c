void MediaElementAudioSourceNode::trace(Visitor* visitor)
{
    visitor->trace(m_mediaElement);
    AudioSourceNode::trace(visitor);
    AudioSourceProviderClient::trace(visitor);
}
