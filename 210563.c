const WillBeHeapVector<RefPtrWillBeMember<SpeechSynthesisVoice> >& SpeechSynthesis::getVoices()
{
    if (m_voiceList.size())
        return m_voiceList;

    const Vector<RefPtr<PlatformSpeechSynthesisVoice> >& platformVoices = m_platformSpeechSynthesizer->voiceList();
    size_t voiceCount = platformVoices.size();
    for (size_t k = 0; k < voiceCount; k++)
        m_voiceList.append(SpeechSynthesisVoice::create(platformVoices[k]));

    return m_voiceList;
}
