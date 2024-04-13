MediaStreamAudioDestinationNode* AudioContext::createMediaStreamDestination()
{
    return MediaStreamAudioDestinationNode::create(this, 2);
}
