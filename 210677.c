MediaElementAudioSourceNode::MediaElementAudioSourceNode(AudioContext* context, HTMLMediaElement* mediaElement)
    : AudioSourceNode(NodeTypeMediaElementAudioSource, context, context->sampleRate())
    , m_mediaElement(mediaElement)
    , m_sourceNumberOfChannels(0)
    , m_sourceSampleRate(0)
{
    addOutput(AudioNodeOutput::create(this, 2));

    initialize();
}
