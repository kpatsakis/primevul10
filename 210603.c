void AudioContext::clear()
{
    m_liveAudioSummingJunctions.clear();
    m_liveNodes.clear();
    m_destinationNode.clear();
    m_isCleared = true;
}
