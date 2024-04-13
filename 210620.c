OscillatorNode* AudioContext::createOscillator()
{
    ASSERT(isMainThread());

    OscillatorNode* node = OscillatorNode::create(this, m_destinationNode->sampleRate());


    return node;
}
